static int update_dev_stat_item(struct btrfs_trans_handle *trans,
				struct btrfs_device *device)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	struct btrfs_root *dev_root = fs_info->dev_root;
	struct btrfs_path *path;
	struct btrfs_key key;
	struct extent_buffer *eb;
	struct btrfs_dev_stats_item *ptr;
	int ret;
	int i;

	key.objectid = BTRFS_DEV_STATS_OBJECTID;
	key.type = BTRFS_PERSISTENT_ITEM_KEY;
	key.offset = device->devid;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;
	ret = btrfs_search_slot(trans, dev_root, &key, path, -1, 1);
	if (ret < 0) {
		btrfs_warn_in_rcu(fs_info,
			"error %d while searching for dev_stats item for device %s",
			      ret, rcu_str_deref(device->name));
		goto out;
	}

	if (ret == 0 &&
	    btrfs_item_size_nr(path->nodes[0], path->slots[0]) < sizeof(*ptr)) {
		/* need to delete old one and insert a new one */
		ret = btrfs_del_item(trans, dev_root, path);
		if (ret != 0) {
			btrfs_warn_in_rcu(fs_info,
				"delete too small dev_stats item for device %s failed %d",
				      rcu_str_deref(device->name), ret);
			goto out;
		}
		ret = 1;
	}

	if (ret == 1) {
		/* need to insert a new item */
		btrfs_release_path(path);
		ret = btrfs_insert_empty_item(trans, dev_root, path,
					      &key, sizeof(*ptr));
		if (ret < 0) {
			btrfs_warn_in_rcu(fs_info,
				"insert dev_stats item for device %s failed %d",
				rcu_str_deref(device->name), ret);
			goto out;
		}
	}

	eb = path->nodes[0];
	ptr = btrfs_item_ptr(eb, path->slots[0], struct btrfs_dev_stats_item);
	for (i = 0; i < BTRFS_DEV_STAT_VALUES_MAX; i++)
		btrfs_set_dev_stats_value(eb, ptr, i,
					  btrfs_dev_stat_read(device, i));
	btrfs_mark_buffer_dirty(eb);

out:
	btrfs_free_path(path);
	return ret;
}