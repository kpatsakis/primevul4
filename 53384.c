struct device *tty_register_device_attr(struct tty_driver *driver,
				   unsigned index, struct device *device,
				   void *drvdata,
				   const struct attribute_group **attr_grp)
{
	char name[64];
	dev_t devt = MKDEV(driver->major, driver->minor_start) + index;
	struct device *dev = NULL;
	int retval = -ENODEV;
	bool cdev = false;

	if (index >= driver->num) {
		pr_err("%s: Attempt to register invalid tty line number (%d)\n",
		       driver->name, index);
		return ERR_PTR(-EINVAL);
	}

	if (driver->type == TTY_DRIVER_TYPE_PTY)
		pty_line_name(driver, index, name);
	else
		tty_line_name(driver, index, name);

	if (!(driver->flags & TTY_DRIVER_DYNAMIC_ALLOC)) {
		retval = tty_cdev_add(driver, devt, index, 1);
		if (retval)
			goto error;
		cdev = true;
	}

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev) {
		retval = -ENOMEM;
		goto error;
	}

	dev->devt = devt;
	dev->class = tty_class;
	dev->parent = device;
	dev->release = tty_device_create_release;
	dev_set_name(dev, "%s", name);
	dev->groups = attr_grp;
	dev_set_drvdata(dev, drvdata);

	retval = device_register(dev);
	if (retval)
		goto error;

	return dev;

error:
	put_device(dev);
	if (cdev) {
		cdev_del(driver->cdevs[index]);
		driver->cdevs[index] = NULL;
	}
	return ERR_PTR(retval);
}
