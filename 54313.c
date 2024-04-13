static void destruct_tty_driver(struct kref *kref)
{
	struct tty_driver *driver = container_of(kref, struct tty_driver, kref);
	int i;
	struct ktermios *tp;
	void *p;

	if (driver->flags & TTY_DRIVER_INSTALLED) {
		/*
		 * Free the termios and termios_locked structures because
		 * we don't want to get memory leaks when modular tty
		 * drivers are removed from the kernel.
		 */
		for (i = 0; i < driver->num; i++) {
			tp = driver->termios[i];
			if (tp) {
				driver->termios[i] = NULL;
				kfree(tp);
			}
			if (!(driver->flags & TTY_DRIVER_DYNAMIC_DEV))
				tty_unregister_device(driver, i);
		}
		p = driver->ttys;
		proc_tty_unregister_driver(driver);
		driver->ttys = NULL;
		driver->termios = NULL;
		kfree(p);
		cdev_del(&driver->cdev);
	}
	kfree(driver);
}
