static long usbdev_ioctl(struct file *file, unsigned int cmd,
			unsigned long arg)
{
	int ret;

	ret = usbdev_do_ioctl(file, cmd, (void __user *)arg);

	return ret;
}
