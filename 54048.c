static int __init cuse_init(void)
{
	int i, rc;

	/* init conntbl */
	for (i = 0; i < CUSE_CONNTBL_LEN; i++)
		INIT_LIST_HEAD(&cuse_conntbl[i]);

	/* inherit and extend fuse_dev_operations */
	cuse_channel_fops		= fuse_dev_operations;
	cuse_channel_fops.owner		= THIS_MODULE;
	cuse_channel_fops.open		= cuse_channel_open;
	cuse_channel_fops.release	= cuse_channel_release;

	cuse_class = class_create(THIS_MODULE, "cuse");
	if (IS_ERR(cuse_class))
		return PTR_ERR(cuse_class);

	cuse_class->dev_groups = cuse_class_dev_groups;

	rc = misc_register(&cuse_miscdev);
	if (rc) {
		class_destroy(cuse_class);
		return rc;
	}

	return 0;
}
