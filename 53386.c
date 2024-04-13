int tty_release(struct inode *inode, struct file *filp)
{
	struct tty_struct *tty = file_tty(filp);
	struct tty_struct *o_tty = NULL;
	int	do_sleep, final;
	int	idx;
	long	timeout = 0;
	int	once = 1;

	if (tty_paranoia_check(tty, inode, __func__))
		return 0;

	tty_lock(tty);
	check_tty_count(tty, __func__);

	__tty_fasync(-1, filp, 0);

	idx = tty->index;
	if (tty->driver->type == TTY_DRIVER_TYPE_PTY &&
	    tty->driver->subtype == PTY_TYPE_MASTER)
		o_tty = tty->link;

	if (tty_release_checks(tty, idx)) {
		tty_unlock(tty);
		return 0;
	}

	tty_debug_hangup(tty, "releasing (count=%d)\n", tty->count);

	if (tty->ops->close)
		tty->ops->close(tty, filp);

	/* If tty is pty master, lock the slave pty (stable lock order) */
	tty_lock_slave(o_tty);

	/*
	 * Sanity check: if tty->count is going to zero, there shouldn't be
	 * any waiters on tty->read_wait or tty->write_wait.  We test the
	 * wait queues and kick everyone out _before_ actually starting to
	 * close.  This ensures that we won't block while releasing the tty
	 * structure.
	 *
	 * The test for the o_tty closing is necessary, since the master and
	 * slave sides may close in any order.  If the slave side closes out
	 * first, its count will be one, since the master side holds an open.
	 * Thus this test wouldn't be triggered at the time the slave closed,
	 * so we do it now.
	 */
	while (1) {
		do_sleep = 0;

		if (tty->count <= 1) {
			if (waitqueue_active(&tty->read_wait)) {
				wake_up_poll(&tty->read_wait, POLLIN);
				do_sleep++;
			}
			if (waitqueue_active(&tty->write_wait)) {
				wake_up_poll(&tty->write_wait, POLLOUT);
				do_sleep++;
			}
		}
		if (o_tty && o_tty->count <= 1) {
			if (waitqueue_active(&o_tty->read_wait)) {
				wake_up_poll(&o_tty->read_wait, POLLIN);
				do_sleep++;
			}
			if (waitqueue_active(&o_tty->write_wait)) {
				wake_up_poll(&o_tty->write_wait, POLLOUT);
				do_sleep++;
			}
		}
		if (!do_sleep)
			break;

		if (once) {
			once = 0;
			tty_warn(tty, "read/write wait queue active!\n");
		}
		schedule_timeout_killable(timeout);
		if (timeout < 120 * HZ)
			timeout = 2 * timeout + 1;
		else
			timeout = MAX_SCHEDULE_TIMEOUT;
	}

	if (o_tty) {
		if (--o_tty->count < 0) {
			tty_warn(tty, "bad slave count (%d)\n", o_tty->count);
			o_tty->count = 0;
		}
	}
	if (--tty->count < 0) {
		tty_warn(tty, "bad tty->count (%d)\n", tty->count);
		tty->count = 0;
	}

	/*
	 * We've decremented tty->count, so we need to remove this file
	 * descriptor off the tty->tty_files list; this serves two
	 * purposes:
	 *  - check_tty_count sees the correct number of file descriptors
	 *    associated with this tty.
	 *  - do_tty_hangup no longer sees this file descriptor as
	 *    something that needs to be handled for hangups.
	 */
	tty_del_file(filp);

	/*
	 * Perform some housekeeping before deciding whether to return.
	 *
	 * If _either_ side is closing, make sure there aren't any
	 * processes that still think tty or o_tty is their controlling
	 * tty.
	 */
	if (!tty->count) {
		read_lock(&tasklist_lock);
		session_clear_tty(tty->session);
		if (o_tty)
			session_clear_tty(o_tty->session);
		read_unlock(&tasklist_lock);
	}

	/* check whether both sides are closing ... */
	final = !tty->count && !(o_tty && o_tty->count);

	tty_unlock_slave(o_tty);
	tty_unlock(tty);

	/* At this point, the tty->count == 0 should ensure a dead tty
	   cannot be re-opened by a racing opener */

	if (!final)
		return 0;

	tty_debug_hangup(tty, "final close\n");
	/*
	 * Ask the line discipline code to release its structures
	 */
	tty_ldisc_release(tty);

	/* Wait for pending work before tty destruction commmences */
	tty_flush_works(tty);

	tty_debug_hangup(tty, "freeing structure\n");
	/*
	 * The release_tty function takes care of the details of clearing
	 * the slots and preserving the termios structure. The tty_unlock_pair
	 * should be safe as we keep a kref while the tty is locked (so the
	 * unlock never unlocks a freed tty).
	 */
	mutex_lock(&tty_mutex);
	release_tty(tty, idx);
	mutex_unlock(&tty_mutex);

	return 0;
}
