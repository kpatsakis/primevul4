md_attr_store(struct kobject *kobj, struct attribute *attr,
	      const char *page, size_t length)
{
	struct md_sysfs_entry *entry = container_of(attr, struct md_sysfs_entry, attr);
	struct mddev *mddev = container_of(kobj, struct mddev, kobj);
	ssize_t rv;

	if (!entry->store)
		return -EIO;
	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;
	spin_lock(&all_mddevs_lock);
	if (list_empty(&mddev->all_mddevs)) {
		spin_unlock(&all_mddevs_lock);
		return -EBUSY;
	}
	mddev_get(mddev);
	spin_unlock(&all_mddevs_lock);
	rv = entry->store(mddev, page, length);
	mddev_put(mddev);
	return rv;
}
