int __init tty_init(void)
{
	cdev_init(&tty_cdev, &tty_fops);
	if (cdev_add(&tty_cdev, MKDEV(TTYAUX_MAJOR, 0), 1) ||
	    register_chrdev_region(MKDEV(TTYAUX_MAJOR, 0), 1, "/dev/tty") < 0)
		panic("Couldn't register /dev/tty driver\n");
	device_create(tty_class, NULL, MKDEV(TTYAUX_MAJOR, 0), NULL, "tty");

	cdev_init(&console_cdev, &console_fops);
	if (cdev_add(&console_cdev, MKDEV(TTYAUX_MAJOR, 1), 1) ||
	    register_chrdev_region(MKDEV(TTYAUX_MAJOR, 1), 1, "/dev/console") < 0)
		panic("Couldn't register /dev/console driver\n");
	consdev = device_create(tty_class, NULL, MKDEV(TTYAUX_MAJOR, 1), NULL,
			      "console");
	if (IS_ERR(consdev))
		consdev = NULL;
	else
		WARN_ON(device_create_file(consdev, &dev_attr_active) < 0);

#ifdef CONFIG_VT
	vty_init(&console_fops);
#endif
	return 0;
}
