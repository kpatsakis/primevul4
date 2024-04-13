int tty_standard_install(struct tty_driver *driver, struct tty_struct *tty)
{
	int ret = tty_init_termios(tty);
	if (ret)
		return ret;

	tty_driver_kref_get(driver);
	tty->count++;
	driver->ttys[tty->index] = tty;
	return 0;
}
