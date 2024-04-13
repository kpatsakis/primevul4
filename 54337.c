static int tty_driver_install_tty(struct tty_driver *driver,
						struct tty_struct *tty)
{
	int idx = tty->index;
	int ret;

	if (driver->ops->install) {
		ret = driver->ops->install(driver, tty);
		return ret;
	}

	if (tty_init_termios(tty) == 0) {
		tty_driver_kref_get(driver);
		tty->count++;
		driver->ttys[idx] = tty;
		return 0;
	}
	return -ENOMEM;
}
