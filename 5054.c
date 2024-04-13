static int btrfs_device_init_dev_stats(struct btrfs_device *device,
				       struct btrfs_path *path)
{
	struct btrfs_dev_stats_item *ptr;
	struct extent_buffer *eb;
	struct btrfs_key key;
	int item_size;
	int i, ret, slot;

	if (!device->fs_info->dev_root)
		return 0;

	key.objectid = BTRFS_DEV_STATS_OBJECTID;
	key.type = BTRFS_PERSISTENT_ITEM_KEY;
	key.offset = device->devid;
	ret = btrfs_search_slot(NULL, device->fs_info->dev_root, &key, path, 0, 0);
	if (ret) {
		for (i = 0; i < BTRFS_DEV_STAT_VALUES_MAX; i++)
			btrfs_dev_stat_set(device, i, 0);
		device->dev_stats_valid = 1;
		btrfs_release_path(path);
		return ret < 0 ? ret : 0;
	}
	slot = path->slots[0];
	eb = path->nodes[0];
	item_size = btrfs_item_size_nr(eb, slot);

	ptr = btrfs_item_ptr(eb, slot, struct btrfs_dev_stats_item);

	for (i = 0; i < BTRFS_DEV_STAT_VALUES_MAX; i++) {
		if (item_size >= (1 + i) * sizeof(__le64))
			btrfs_dev_stat_set(device, i,
					   btrfs_dev_stats_value(eb, ptr, i));
		else
			btrfs_dev_stat_set(device, i, 0);
	}

	device->dev_stats_valid = 1;
	btrfs_dev_stat_print_on_load(device);
	btrfs_release_path(path);

	return 0;
}