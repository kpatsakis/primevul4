static void tty_driver_remove_tty(struct tty_driver *driver,
						struct tty_struct *tty)
{
	if (driver->ops->remove)
		driver->ops->remove(driver, tty);
	else
		driver->ttys[tty->index] = NULL;
}
