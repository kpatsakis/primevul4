static long media_device_compat_ioctl(struct file *filp, unsigned int cmd,
				      unsigned long arg)
{
	struct media_devnode *devnode = media_devnode_data(filp);
	struct media_device *dev = to_media_device(devnode);
	long ret;

	switch (cmd) {
	case MEDIA_IOC_DEVICE_INFO:
	case MEDIA_IOC_ENUM_ENTITIES:
	case MEDIA_IOC_SETUP_LINK:
		return media_device_ioctl(filp, cmd, arg);

	case MEDIA_IOC_ENUM_LINKS32:
		mutex_lock(&dev->graph_mutex);
		ret = media_device_enum_links32(dev,
				(struct media_links_enum32 __user *)arg);
		mutex_unlock(&dev->graph_mutex);
		break;

	default:
		ret = -ENOIOCTLCMD;
	}

	return ret;
}
