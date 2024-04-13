static int __init comedi_init(void)
{
	int i;
	int retval;

	printk(KERN_INFO "comedi: version " COMEDI_RELEASE
	       " - http://www.comedi.org\n");

	if (comedi_num_legacy_minors < 0 ||
	    comedi_num_legacy_minors > COMEDI_NUM_BOARD_MINORS) {
		printk(KERN_ERR "comedi: error: invalid value for module "
		       "parameter \"comedi_num_legacy_minors\".  Valid values "
		       "are 0 through %i.\n", COMEDI_NUM_BOARD_MINORS);
		return -EINVAL;
	}

	/*
	 * comedi is unusable if both comedi_autoconfig and
	 * comedi_num_legacy_minors are zero, so we might as well adjust the
	 * defaults in that case
	 */
	if (comedi_autoconfig == 0 && comedi_num_legacy_minors == 0)
		comedi_num_legacy_minors = 16;

	memset(comedi_file_info_table, 0,
	       sizeof(struct comedi_device_file_info *) * COMEDI_NUM_MINORS);

	retval = register_chrdev_region(MKDEV(COMEDI_MAJOR, 0),
					COMEDI_NUM_MINORS, "comedi");
	if (retval)
		return -EIO;
	cdev_init(&comedi_cdev, &comedi_fops);
	comedi_cdev.owner = THIS_MODULE;
	kobject_set_name(&comedi_cdev.kobj, "comedi");
	if (cdev_add(&comedi_cdev, MKDEV(COMEDI_MAJOR, 0), COMEDI_NUM_MINORS)) {
		unregister_chrdev_region(MKDEV(COMEDI_MAJOR, 0),
					 COMEDI_NUM_MINORS);
		return -EIO;
	}
	comedi_class = class_create(THIS_MODULE, "comedi");
	if (IS_ERR(comedi_class)) {
		printk(KERN_ERR "comedi: failed to create class");
		cdev_del(&comedi_cdev);
		unregister_chrdev_region(MKDEV(COMEDI_MAJOR, 0),
					 COMEDI_NUM_MINORS);
		return PTR_ERR(comedi_class);
	}

	/* XXX requires /proc interface */
	comedi_proc_init();

	/* create devices files for legacy/manual use */
	for (i = 0; i < comedi_num_legacy_minors; i++) {
		int minor;
		minor = comedi_alloc_board_minor(NULL);
		if (minor < 0) {
			comedi_cleanup_legacy_minors();
			cdev_del(&comedi_cdev);
			unregister_chrdev_region(MKDEV(COMEDI_MAJOR, 0),
						 COMEDI_NUM_MINORS);
			return minor;
		}
	}

	return 0;
}
