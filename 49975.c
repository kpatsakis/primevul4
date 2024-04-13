static int __init bsg_init(void)
{
	int ret, i;
	dev_t devid;

	bsg_cmd_cachep = kmem_cache_create("bsg_cmd",
				sizeof(struct bsg_command), 0, 0, NULL);
	if (!bsg_cmd_cachep) {
		printk(KERN_ERR "bsg: failed creating slab cache\n");
		return -ENOMEM;
	}

	for (i = 0; i < BSG_LIST_ARRAY_SIZE; i++)
		INIT_HLIST_HEAD(&bsg_device_list[i]);

	bsg_class = class_create(THIS_MODULE, "bsg");
	if (IS_ERR(bsg_class)) {
		ret = PTR_ERR(bsg_class);
		goto destroy_kmemcache;
	}
	bsg_class->devnode = bsg_devnode;

	ret = alloc_chrdev_region(&devid, 0, BSG_MAX_DEVS, "bsg");
	if (ret)
		goto destroy_bsg_class;

	bsg_major = MAJOR(devid);

	cdev_init(&bsg_cdev, &bsg_fops);
	ret = cdev_add(&bsg_cdev, MKDEV(bsg_major, 0), BSG_MAX_DEVS);
	if (ret)
		goto unregister_chrdev;

	printk(KERN_INFO BSG_DESCRIPTION " version " BSG_VERSION
	       " loaded (major %d)\n", bsg_major);
	return 0;
unregister_chrdev:
	unregister_chrdev_region(MKDEV(bsg_major, 0), BSG_MAX_DEVS);
destroy_bsg_class:
	class_destroy(bsg_class);
destroy_kmemcache:
	kmem_cache_destroy(bsg_cmd_cachep);
	return ret;
}
