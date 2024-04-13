int tty_register_driver(struct tty_driver *driver)
{
	int error;
	int i;
	dev_t dev;
	void **p = NULL;
	struct device *d;

	if (!(driver->flags & TTY_DRIVER_DEVPTS_MEM) && driver->num) {
		p = kzalloc(driver->num * 2 * sizeof(void *), GFP_KERNEL);
		if (!p)
			return -ENOMEM;
	}

	if (!driver->major) {
		error = alloc_chrdev_region(&dev, driver->minor_start,
						driver->num, driver->name);
		if (!error) {
			driver->major = MAJOR(dev);
			driver->minor_start = MINOR(dev);
		}
	} else {
		dev = MKDEV(driver->major, driver->minor_start);
		error = register_chrdev_region(dev, driver->num, driver->name);
	}
	if (error < 0) {
		kfree(p);
		return error;
	}

	if (p) {
		driver->ttys = (struct tty_struct **)p;
		driver->termios = (struct ktermios **)(p + driver->num);
	} else {
		driver->ttys = NULL;
		driver->termios = NULL;
	}

	cdev_init(&driver->cdev, &tty_fops);
	driver->cdev.owner = driver->owner;
	error = cdev_add(&driver->cdev, dev, driver->num);
	if (error) {
		unregister_chrdev_region(dev, driver->num);
		driver->ttys = NULL;
		driver->termios = NULL;
		kfree(p);
		return error;
	}

	mutex_lock(&tty_mutex);
	list_add(&driver->tty_drivers, &tty_drivers);
	mutex_unlock(&tty_mutex);

	if (!(driver->flags & TTY_DRIVER_DYNAMIC_DEV)) {
		for (i = 0; i < driver->num; i++) {
			d = tty_register_device(driver, i, NULL);
			if (IS_ERR(d)) {
				error = PTR_ERR(d);
				goto err;
			}
		}
	}
	proc_tty_register_driver(driver);
	driver->flags |= TTY_DRIVER_INSTALLED;
	return 0;

err:
	for (i--; i >= 0; i--)
		tty_unregister_device(driver, i);

	mutex_lock(&tty_mutex);
	list_del(&driver->tty_drivers);
	mutex_unlock(&tty_mutex);

	unregister_chrdev_region(dev, driver->num);
	driver->ttys = NULL;
	driver->termios = NULL;
	kfree(p);
	return error;
}
