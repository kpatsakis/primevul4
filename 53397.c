int tty_unregister_driver(struct tty_driver *driver)
{
#if 0
	/* FIXME */
	if (driver->refcount)
		return -EBUSY;
#endif
	unregister_chrdev_region(MKDEV(driver->major, driver->minor_start),
				driver->num);
	mutex_lock(&tty_mutex);
	list_del(&driver->tty_drivers);
	mutex_unlock(&tty_mutex);
	return 0;
}
