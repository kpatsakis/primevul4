static int adf_ctl_ioctl_get_num_devices(struct file *fp, unsigned int cmd,
					 unsigned long arg)
{
	uint32_t num_devices = 0;

	adf_devmgr_get_num_dev(&num_devices);
	if (copy_to_user((void __user *)arg, &num_devices, sizeof(num_devices)))
		return -EFAULT;

	return 0;
}
