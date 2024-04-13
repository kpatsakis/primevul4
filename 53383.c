struct device *tty_register_device(struct tty_driver *driver, unsigned index,
				   struct device *device)
{
	return tty_register_device_attr(driver, index, device, NULL, NULL);
}
