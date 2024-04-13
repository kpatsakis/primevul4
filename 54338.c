static struct tty_struct *tty_driver_lookup_tty(struct tty_driver *driver,
		struct inode *inode, int idx)
{
	struct tty_struct *tty;

	if (driver->ops->lookup)
		return driver->ops->lookup(driver, inode, idx);

	tty = driver->ttys[idx];
	return tty;
}
