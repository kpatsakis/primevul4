static int usbdev_release(struct inode *inode, struct file *file)
{
	struct usb_dev_state *ps = file->private_data;
	struct usb_device *dev = ps->dev;
	unsigned int ifnum;
	struct async *as;

	usb_lock_device(dev);
	usb_hub_release_all_ports(dev, ps);

	list_del_init(&ps->list);

	for (ifnum = 0; ps->ifclaimed && ifnum < 8*sizeof(ps->ifclaimed);
			ifnum++) {
		if (test_bit(ifnum, &ps->ifclaimed))
			releaseintf(ps, ifnum);
	}
	destroy_all_async(ps);
	usb_autosuspend_device(dev);
	usb_unlock_device(dev);
	usb_put_dev(dev);
	put_pid(ps->disc_pid);
	put_cred(ps->cred);

	as = async_getcompleted(ps);
	while (as) {
		free_async(as);
		as = async_getcompleted(ps);
	}

	kfree(ps);
	return 0;
}
