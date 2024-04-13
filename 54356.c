static int tty_reopen(struct tty_struct *tty)
{
	struct tty_driver *driver = tty->driver;

	if (test_bit(TTY_CLOSING, &tty->flags) ||
			test_bit(TTY_HUPPING, &tty->flags) ||
			test_bit(TTY_LDISC_CHANGING, &tty->flags))
		return -EIO;

	if (driver->type == TTY_DRIVER_TYPE_PTY &&
	    driver->subtype == PTY_TYPE_MASTER) {
		/*
		 * special case for PTY masters: only one open permitted,
		 * and the slave side open count is incremented as well.
		 */
		if (tty->count)
			return -EIO;

		tty->link->count++;
	}
	tty->count++;
	tty->driver = driver; /* N.B. why do this every time?? */

	mutex_lock(&tty->ldisc_mutex);
	WARN_ON(!test_bit(TTY_LDISC, &tty->flags));
	mutex_unlock(&tty->ldisc_mutex);

	return 0;
}
