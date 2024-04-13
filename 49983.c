int bsg_register_queue(struct request_queue *q, struct device *parent,
		       const char *name, void (*release)(struct device *))
{
	struct bsg_class_device *bcd;
	dev_t dev;
	int ret;
	struct device *class_dev = NULL;
	const char *devname;

	if (name)
		devname = name;
	else
		devname = dev_name(parent);

	/*
	 * we need a proper transport to send commands, not a stacked device
	 */
	if (!queue_is_rq_based(q))
		return 0;

	bcd = &q->bsg_dev;
	memset(bcd, 0, sizeof(*bcd));

	mutex_lock(&bsg_mutex);

	ret = idr_alloc(&bsg_minor_idr, bcd, 0, BSG_MAX_DEVS, GFP_KERNEL);
	if (ret < 0) {
		if (ret == -ENOSPC) {
			printk(KERN_ERR "bsg: too many bsg devices\n");
			ret = -EINVAL;
		}
		goto unlock;
	}

	bcd->minor = ret;
	bcd->queue = q;
	bcd->parent = get_device(parent);
	bcd->release = release;
	kref_init(&bcd->ref);
	dev = MKDEV(bsg_major, bcd->minor);
	class_dev = device_create(bsg_class, parent, dev, NULL, "%s", devname);
	if (IS_ERR(class_dev)) {
		ret = PTR_ERR(class_dev);
		goto put_dev;
	}
	bcd->class_dev = class_dev;

	if (q->kobj.sd) {
		ret = sysfs_create_link(&q->kobj, &bcd->class_dev->kobj, "bsg");
		if (ret)
			goto unregister_class_dev;
	}

	mutex_unlock(&bsg_mutex);
	return 0;

unregister_class_dev:
	device_unregister(class_dev);
put_dev:
	put_device(parent);
	idr_remove(&bsg_minor_idr, bcd->minor);
unlock:
	mutex_unlock(&bsg_mutex);
	return ret;
}
