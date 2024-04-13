static long media_device_ioctl(struct file *filp, unsigned int cmd,
			       unsigned long arg)
{
	struct media_devnode *devnode = media_devnode_data(filp);
	struct media_device *dev = to_media_device(devnode);
	long ret;

	switch (cmd) {
	case MEDIA_IOC_DEVICE_INFO:
		ret = media_device_get_info(dev,
				(struct media_device_info __user *)arg);
		break;

	case MEDIA_IOC_ENUM_ENTITIES:
		ret = media_device_enum_entities(dev,
				(struct media_entity_desc __user *)arg);
		break;

	case MEDIA_IOC_ENUM_LINKS:
		mutex_lock(&dev->graph_mutex);
		ret = media_device_enum_links(dev,
				(struct media_links_enum __user *)arg);
		mutex_unlock(&dev->graph_mutex);
		break;

	case MEDIA_IOC_SETUP_LINK:
		mutex_lock(&dev->graph_mutex);
		ret = media_device_setup_link(dev,
				(struct media_link_desc __user *)arg);
		mutex_unlock(&dev->graph_mutex);
		break;

	default:
		ret = -ENOIOCTLCMD;
	}

	return ret;
}
