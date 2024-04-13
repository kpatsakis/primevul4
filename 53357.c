static struct tty_struct *tty_driver_lookup_tty(struct tty_driver *driver,
		struct inode *inode, int idx)
{
	struct tty_struct *tty;

	if (driver->ops->lookup)
		tty = driver->ops->lookup(driver, inode, idx);
	else
		tty = driver->ttys[idx];

	if (!IS_ERR(tty))
		tty_kref_get(tty);
	return tty;
}
