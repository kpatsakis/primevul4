static void __exit comedi_cleanup(void)
{
	int i;

	comedi_cleanup_legacy_minors();
	for (i = 0; i < COMEDI_NUM_MINORS; ++i)
		BUG_ON(comedi_file_info_table[i]);

	class_destroy(comedi_class);
	cdev_del(&comedi_cdev);
	unregister_chrdev_region(MKDEV(COMEDI_MAJOR, 0), COMEDI_NUM_MINORS);

	comedi_proc_cleanup();
}
