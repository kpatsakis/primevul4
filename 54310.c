struct tty_driver *alloc_tty_driver(int lines)
{
	struct tty_driver *driver;

	driver = kzalloc(sizeof(struct tty_driver), GFP_KERNEL);
	if (driver) {
		kref_init(&driver->kref);
		driver->magic = TTY_DRIVER_MAGIC;
		driver->num = lines;
		/* later we'll move allocation of tables here */
	}
	return driver;
}
