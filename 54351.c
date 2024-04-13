static unsigned int tty_poll(struct file *filp, poll_table *wait)
{
	struct tty_struct *tty = file_tty(filp);
	struct tty_ldisc *ld;
	int ret = 0;

	if (tty_paranoia_check(tty, filp->f_path.dentry->d_inode, "tty_poll"))
		return 0;

	ld = tty_ldisc_ref_wait(tty);
	if (ld->ops->poll)
		ret = (ld->ops->poll)(tty, filp, wait);
	tty_ldisc_deref(ld);
	return ret;
}
