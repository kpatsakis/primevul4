struct device *tty_register_device(struct tty_driver *driver, unsigned index,
				   struct device *device)
{
	char name[64];
	dev_t dev = MKDEV(driver->major, driver->minor_start) + index;

	if (index >= driver->num) {
		printk(KERN_ERR "Attempt to register invalid tty line number "
		       " (%d).\n", index);
		return ERR_PTR(-EINVAL);
	}

	if (driver->type == TTY_DRIVER_TYPE_PTY)
		pty_line_name(driver, index, name);
	else
		tty_line_name(driver, index, name);

	return device_create(tty_class, device, dev, NULL, name);
}
