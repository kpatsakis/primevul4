int btrfs_forget_devices(const char *path)
{
	int ret;

	mutex_lock(&uuid_mutex);
	ret = btrfs_free_stale_devices(strlen(path) ? path : NULL, NULL);
	mutex_unlock(&uuid_mutex);

	return ret;
}