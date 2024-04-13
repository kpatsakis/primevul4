static noinline int hiddev_ioctl_string(struct hiddev *hiddev, unsigned int cmd, void __user *user_arg)
{
	struct hid_device *hid = hiddev->hid;
	struct usb_device *dev = hid_to_usb_dev(hid);
	int idx, len;
	char *buf;

	if (get_user(idx, (int __user *)user_arg))
		return -EFAULT;

	if ((buf = kmalloc(HID_STRING_SIZE, GFP_KERNEL)) == NULL)
		return -ENOMEM;

	if ((len = usb_string(dev, idx, buf, HID_STRING_SIZE-1)) < 0) {
		kfree(buf);
		return -EINVAL;
	}

	if (copy_to_user(user_arg+sizeof(int), buf, len+1)) {
		kfree(buf);
		return -EFAULT;
	}

	kfree(buf);

	return len;
}
