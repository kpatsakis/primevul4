static bool device_path_matched(const char *path, struct btrfs_device *device)
{
	int found;

	rcu_read_lock();
	found = strcmp(rcu_str_deref(device->name), path);
	rcu_read_unlock();

	return found == 0;
}