static void cuse_process_init_reply(struct fuse_conn *fc, struct fuse_req *req)
{
	struct cuse_conn *cc = fc_to_cc(fc), *pos;
	struct cuse_init_out *arg = req->out.args[0].value;
	struct page *page = req->pages[0];
	struct cuse_devinfo devinfo = { };
	struct device *dev;
	struct cdev *cdev;
	dev_t devt;
	int rc, i;

	if (req->out.h.error ||
	    arg->major != FUSE_KERNEL_VERSION || arg->minor < 11) {
		goto err;
	}

	fc->minor = arg->minor;
	fc->max_read = max_t(unsigned, arg->max_read, 4096);
	fc->max_write = max_t(unsigned, arg->max_write, 4096);

	/* parse init reply */
	cc->unrestricted_ioctl = arg->flags & CUSE_UNRESTRICTED_IOCTL;

	rc = cuse_parse_devinfo(page_address(page), req->out.args[1].size,
				&devinfo);
	if (rc)
		goto err;

	/* determine and reserve devt */
	devt = MKDEV(arg->dev_major, arg->dev_minor);
	if (!MAJOR(devt))
		rc = alloc_chrdev_region(&devt, MINOR(devt), 1, devinfo.name);
	else
		rc = register_chrdev_region(devt, 1, devinfo.name);
	if (rc) {
		printk(KERN_ERR "CUSE: failed to register chrdev region\n");
		goto err;
	}

	/* devt determined, create device */
	rc = -ENOMEM;
	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		goto err_region;

	device_initialize(dev);
	dev_set_uevent_suppress(dev, 1);
	dev->class = cuse_class;
	dev->devt = devt;
	dev->release = cuse_gendev_release;
	dev_set_drvdata(dev, cc);
	dev_set_name(dev, "%s", devinfo.name);

	mutex_lock(&cuse_lock);

	/* make sure the device-name is unique */
	for (i = 0; i < CUSE_CONNTBL_LEN; ++i) {
		list_for_each_entry(pos, &cuse_conntbl[i], list)
			if (!strcmp(dev_name(pos->dev), dev_name(dev)))
				goto err_unlock;
	}

	rc = device_add(dev);
	if (rc)
		goto err_unlock;

	/* register cdev */
	rc = -ENOMEM;
	cdev = cdev_alloc();
	if (!cdev)
		goto err_unlock;

	cdev->owner = THIS_MODULE;
	cdev->ops = &cuse_frontend_fops;

	rc = cdev_add(cdev, devt, 1);
	if (rc)
		goto err_cdev;

	cc->dev = dev;
	cc->cdev = cdev;

	/* make the device available */
	list_add(&cc->list, cuse_conntbl_head(devt));
	mutex_unlock(&cuse_lock);

	/* announce device availability */
	dev_set_uevent_suppress(dev, 0);
	kobject_uevent(&dev->kobj, KOBJ_ADD);
out:
	kfree(arg);
	__free_page(page);
	return;

err_cdev:
	cdev_del(cdev);
err_unlock:
	mutex_unlock(&cuse_lock);
	put_device(dev);
err_region:
	unregister_chrdev_region(devt, 1);
err:
	fuse_abort_conn(fc);
	goto out;
}
