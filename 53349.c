static int __init tty_class_init(void)
{
	tty_class = class_create(THIS_MODULE, "tty");
	if (IS_ERR(tty_class))
		return PTR_ERR(tty_class);
	tty_class->devnode = tty_devnode;
	return 0;
}
