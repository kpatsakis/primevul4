static struct tty_struct *tty_open_current_tty(dev_t device, struct file *filp)
{
	struct tty_struct *tty;
	int retval;

	if (device != MKDEV(TTYAUX_MAJOR, 0))
		return NULL;

	tty = get_current_tty();
	if (!tty)
		return ERR_PTR(-ENXIO);

	filp->f_flags |= O_NONBLOCK; /* Don't let /dev/tty block */
	/* noctty = 1; */
	tty_lock(tty);
	tty_kref_put(tty);	/* safe to drop the kref now */

	retval = tty_reopen(tty);
	if (retval < 0) {
		tty_unlock(tty);
		tty = ERR_PTR(retval);
	}
	return tty;
}
