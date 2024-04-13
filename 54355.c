int tty_release(struct inode *inode, struct file *filp)
{
	struct tty_struct *tty = file_tty(filp);
	struct tty_struct *o_tty;
	int	pty_master, tty_closing, o_tty_closing, do_sleep;
	int	devpts;
	int	idx;
	char	buf[64];

	if (tty_paranoia_check(tty, inode, "tty_release_dev"))
		return 0;

	tty_lock();
	check_tty_count(tty, "tty_release_dev");

	__tty_fasync(-1, filp, 0);

	idx = tty->index;
	pty_master = (tty->driver->type == TTY_DRIVER_TYPE_PTY &&
		      tty->driver->subtype == PTY_TYPE_MASTER);
	devpts = (tty->driver->flags & TTY_DRIVER_DEVPTS_MEM) != 0;
	o_tty = tty->link;

#ifdef TTY_PARANOIA_CHECK
	if (idx < 0 || idx >= tty->driver->num) {
		printk(KERN_DEBUG "tty_release_dev: bad idx when trying to "
				  "free (%s)\n", tty->name);
		tty_unlock();
		return 0;
	}
	if (!devpts) {
		if (tty != tty->driver->ttys[idx]) {
			tty_unlock();
			printk(KERN_DEBUG "tty_release_dev: driver.table[%d] not tty "
			       "for (%s)\n", idx, tty->name);
			return 0;
		}
		if (tty->termios != tty->driver->termios[idx]) {
			tty_unlock();
			printk(KERN_DEBUG "tty_release_dev: driver.termios[%d] not termios "
			       "for (%s)\n",
			       idx, tty->name);
			return 0;
		}
	}
#endif

#ifdef TTY_DEBUG_HANGUP
	printk(KERN_DEBUG "tty_release_dev of %s (tty count=%d)...",
	       tty_name(tty, buf), tty->count);
#endif

#ifdef TTY_PARANOIA_CHECK
	if (tty->driver->other &&
	     !(tty->driver->flags & TTY_DRIVER_DEVPTS_MEM)) {
		if (o_tty != tty->driver->other->ttys[idx]) {
			tty_unlock();
			printk(KERN_DEBUG "tty_release_dev: other->table[%d] "
					  "not o_tty for (%s)\n",
			       idx, tty->name);
			return 0 ;
		}
		if (o_tty->termios != tty->driver->other->termios[idx]) {
			tty_unlock();
			printk(KERN_DEBUG "tty_release_dev: other->termios[%d] "
					  "not o_termios for (%s)\n",
			       idx, tty->name);
			return 0;
		}
		if (o_tty->link != tty) {
			tty_unlock();
			printk(KERN_DEBUG "tty_release_dev: bad pty pointers\n");
			return 0;
		}
	}
#endif
	if (tty->ops->close)
		tty->ops->close(tty, filp);

	tty_unlock();
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
	 * Thus this test wouldn't be triggered at the time the slave closes,
	 * so we do it now.
	 *
	 * Note that it's possible for the tty to be opened again while we're
	 * flushing out waiters.  By recalculating the closing flags before
	 * each iteration we avoid any problems.
	 */
	while (1) {
		/* Guard against races with tty->count changes elsewhere and
		   opens on /dev/tty */

		mutex_lock(&tty_mutex);
		tty_lock();
		tty_closing = tty->count <= 1;
		o_tty_closing = o_tty &&
			(o_tty->count <= (pty_master ? 1 : 0));
		do_sleep = 0;

		if (tty_closing) {
			if (waitqueue_active(&tty->read_wait)) {
				wake_up_poll(&tty->read_wait, POLLIN);
				do_sleep++;
			}
			if (waitqueue_active(&tty->write_wait)) {
				wake_up_poll(&tty->write_wait, POLLOUT);
				do_sleep++;
			}
		}
		if (o_tty_closing) {
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

		printk(KERN_WARNING "tty_release_dev: %s: read/write wait queue "
				    "active!\n", tty_name(tty, buf));
		tty_unlock();
		mutex_unlock(&tty_mutex);
		schedule();
	}

	/*
	 * The closing flags are now consistent with the open counts on
	 * both sides, and we've completed the last operation that could
	 * block, so it's safe to proceed with closing.
	 */
	if (pty_master) {
		if (--o_tty->count < 0) {
			printk(KERN_WARNING "tty_release_dev: bad pty slave count "
					    "(%d) for %s\n",
			       o_tty->count, tty_name(o_tty, buf));
			o_tty->count = 0;
		}
	}
	if (--tty->count < 0) {
		printk(KERN_WARNING "tty_release_dev: bad tty->count (%d) for %s\n",
		       tty->count, tty_name(tty, buf));
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
	 * Set the TTY_CLOSING flag if this was the last open.  In the
	 * case of a pty we may have to wait around for the other side
	 * to close, and TTY_CLOSING makes sure we can't be reopened.
	 */
	if (tty_closing)
		set_bit(TTY_CLOSING, &tty->flags);
	if (o_tty_closing)
		set_bit(TTY_CLOSING, &o_tty->flags);

	/*
	 * If _either_ side is closing, make sure there aren't any
	 * processes that still think tty or o_tty is their controlling
	 * tty.
	 */
	if (tty_closing || o_tty_closing) {
		read_lock(&tasklist_lock);
		session_clear_tty(tty->session);
		if (o_tty)
			session_clear_tty(o_tty->session);
		read_unlock(&tasklist_lock);
	}

	mutex_unlock(&tty_mutex);

	/* check whether both sides are closing ... */
	if (!tty_closing || (o_tty && !o_tty_closing)) {
		tty_unlock();
		return 0;
	}

#ifdef TTY_DEBUG_HANGUP
	printk(KERN_DEBUG "freeing tty structure...");
#endif
	/*
	 * Ask the line discipline code to release its structures
	 */
	tty_ldisc_release(tty, o_tty);
	/*
	 * The release_tty function takes care of the details of clearing
	 * the slots and preserving the termios structure.
	 */
	release_tty(tty, idx);

	/* Make this pty number available for reallocation */
	if (devpts)
		devpts_kill_index(inode, idx);
	tty_unlock();
	return 0;
}
