static int adf_chr_drv_create(void)
{
	dev_t dev_id;
	struct device *drv_device;

	if (alloc_chrdev_region(&dev_id, 0, 1, DEVICE_NAME)) {
		pr_err("QAT: unable to allocate chrdev region\n");
		return -EFAULT;
	}

	adt_ctl_drv.drv_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(adt_ctl_drv.drv_class)) {
		pr_err("QAT: class_create failed for adf_ctl\n");
		goto err_chrdev_unreg;
	}
	adt_ctl_drv.major = MAJOR(dev_id);
	cdev_init(&adt_ctl_drv.drv_cdev, &adf_ctl_ops);
	if (cdev_add(&adt_ctl_drv.drv_cdev, dev_id, 1)) {
		pr_err("QAT: cdev add failed\n");
		goto err_class_destr;
	}

	drv_device = device_create(adt_ctl_drv.drv_class, NULL,
				   MKDEV(adt_ctl_drv.major, 0),
				   NULL, DEVICE_NAME);
	if (IS_ERR(drv_device)) {
		pr_err("QAT: failed to create device\n");
		goto err_cdev_del;
	}
	return 0;
err_cdev_del:
	cdev_del(&adt_ctl_drv.drv_cdev);
err_class_destr:
	class_destroy(adt_ctl_drv.drv_class);
err_chrdev_unreg:
	unregister_chrdev_region(dev_id, 1);
	return -EFAULT;
}
