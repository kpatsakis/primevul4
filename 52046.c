static int usbdev_open(struct inode *inode, struct file *file)
{
	struct usb_device *dev = NULL;
	struct usb_dev_state *ps;
	int ret;

	ret = -ENOMEM;
	ps = kzalloc(sizeof(struct usb_dev_state), GFP_KERNEL);
	if (!ps)
		goto out_free_ps;

	ret = -ENODEV;

	/* Protect against simultaneous removal or release */
	mutex_lock(&usbfs_mutex);

	/* usbdev device-node */
	if (imajor(inode) == USB_DEVICE_MAJOR)
		dev = usbdev_lookup_by_devt(inode->i_rdev);

	mutex_unlock(&usbfs_mutex);

	if (!dev)
		goto out_free_ps;

	usb_lock_device(dev);
	if (dev->state == USB_STATE_NOTATTACHED)
		goto out_unlock_device;

	ret = usb_autoresume_device(dev);
	if (ret)
		goto out_unlock_device;

	ps->dev = dev;
	ps->file = file;
	ps->interface_allowed_mask = 0xFFFFFFFF; /* 32 bits */
	spin_lock_init(&ps->lock);
	INIT_LIST_HEAD(&ps->list);
	INIT_LIST_HEAD(&ps->async_pending);
	INIT_LIST_HEAD(&ps->async_completed);
	INIT_LIST_HEAD(&ps->memory_list);
	init_waitqueue_head(&ps->wait);
	ps->disc_pid = get_pid(task_pid(current));
	ps->cred = get_current_cred();
	security_task_getsecid(current, &ps->secid);
	smp_wmb();
	list_add_tail(&ps->list, &dev->filelist);
	file->private_data = ps;
	usb_unlock_device(dev);
	snoop(&dev->dev, "opened by process %d: %s\n", task_pid_nr(current),
			current->comm);
	return ret;

 out_unlock_device:
	usb_unlock_device(dev);
	usb_put_dev(dev);
 out_free_ps:
	kfree(ps);
	return ret;
}
