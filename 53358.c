const char *tty_driver_name(const struct tty_struct *tty)
{
	if (!tty || !tty->driver)
		return "";
	return tty->driver->name;
}
