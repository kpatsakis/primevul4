static int tty_reopen(struct tty_struct *tty)
{
	struct tty_driver *driver = tty->driver;

	if (driver->type == TTY_DRIVER_TYPE_PTY &&
	    driver->subtype == PTY_TYPE_MASTER)
		return -EIO;

	if (!tty->count)
		return -EAGAIN;

	if (test_bit(TTY_EXCLUSIVE, &tty->flags) && !capable(CAP_SYS_ADMIN))
		return -EBUSY;

	tty->count++;

	WARN_ON(!tty->ldisc);

	return 0;
}
