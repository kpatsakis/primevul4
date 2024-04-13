static int __tty_fasync(int fd, struct file *filp, int on)
{
	struct tty_struct *tty = file_tty(filp);
	struct tty_ldisc *ldisc;
	unsigned long flags;
	int retval = 0;

	if (tty_paranoia_check(tty, file_inode(filp), "tty_fasync"))
		goto out;

	retval = fasync_helper(fd, filp, on, &tty->fasync);
	if (retval <= 0)
		goto out;

	ldisc = tty_ldisc_ref(tty);
	if (ldisc) {
		if (ldisc->ops->fasync)
			ldisc->ops->fasync(tty, on);
		tty_ldisc_deref(ldisc);
	}

	if (on) {
		enum pid_type type;
		struct pid *pid;

		spin_lock_irqsave(&tty->ctrl_lock, flags);
		if (tty->pgrp) {
			pid = tty->pgrp;
			type = PIDTYPE_PGID;
		} else {
			pid = task_pid(current);
			type = PIDTYPE_PID;
		}
		get_pid(pid);
		spin_unlock_irqrestore(&tty->ctrl_lock, flags);
		__f_setown(filp, pid, type, 0);
		put_pid(pid);
		retval = 0;
	}
out:
	return retval;
}
