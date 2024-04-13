static long ec_device_ioctl(struct file *filp, unsigned int cmd,
			    unsigned long arg)
{
	struct cros_ec_dev *ec = filp->private_data;

	if (_IOC_TYPE(cmd) != CROS_EC_DEV_IOC)
		return -ENOTTY;

	switch (cmd) {
	case CROS_EC_DEV_IOCXCMD:
		return ec_device_ioctl_xcmd(ec, (void __user *)arg);
	case CROS_EC_DEV_IOCRDMEM:
		return ec_device_ioctl_readmem(ec, (void __user *)arg);
	}

	return -ENOTTY;
}
