static int media_device_get_info(struct media_device *dev,
				 struct media_device_info __user *__info)
{
	struct media_device_info info;

	memset(&info, 0, sizeof(info));

	strlcpy(info.driver, dev->dev->driver->name, sizeof(info.driver));
	strlcpy(info.model, dev->model, sizeof(info.model));
	strlcpy(info.serial, dev->serial, sizeof(info.serial));
	strlcpy(info.bus_info, dev->bus_info, sizeof(info.bus_info));

	info.media_version = MEDIA_API_VERSION;
	info.hw_revision = dev->hw_revision;
	info.driver_version = dev->driver_version;

	if (copy_to_user(__info, &info, sizeof(*__info)))
		return -EFAULT;
	return 0;
}
