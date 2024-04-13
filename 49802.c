static long adf_ctl_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	int ret;

	if (mutex_lock_interruptible(&adf_ctl_lock))
		return -EFAULT;

	switch (cmd) {
	case IOCTL_CONFIG_SYS_RESOURCE_PARAMETERS:
		ret = adf_ctl_ioctl_dev_config(fp, cmd, arg);
		break;

	case IOCTL_STOP_ACCEL_DEV:
		ret = adf_ctl_ioctl_dev_stop(fp, cmd, arg);
		break;

	case IOCTL_START_ACCEL_DEV:
		ret = adf_ctl_ioctl_dev_start(fp, cmd, arg);
		break;

	case IOCTL_GET_NUM_DEVICES:
		ret = adf_ctl_ioctl_get_num_devices(fp, cmd, arg);
		break;

	case IOCTL_STATUS_ACCEL_DEV:
		ret = adf_ctl_ioctl_get_status(fp, cmd, arg);
		break;
	default:
		pr_err("QAT: Invalid ioctl\n");
		ret = -EFAULT;
		break;
	}
	mutex_unlock(&adf_ctl_lock);
	return ret;
}
