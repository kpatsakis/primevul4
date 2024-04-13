struct btrfs_device *btrfs_alloc_device(struct btrfs_fs_info *fs_info,
					const u64 *devid,
					const u8 *uuid)
{
	struct btrfs_device *dev;
	u64 tmp;

	if (WARN_ON(!devid && !fs_info))
		return ERR_PTR(-EINVAL);

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return ERR_PTR(-ENOMEM);

	/*
	 * Preallocate a bio that's always going to be used for flushing device
	 * barriers and matches the device lifespan
	 */
	dev->flush_bio = bio_kmalloc(GFP_KERNEL, 0);
	if (!dev->flush_bio) {
		kfree(dev);
		return ERR_PTR(-ENOMEM);
	}

	INIT_LIST_HEAD(&dev->dev_list);
	INIT_LIST_HEAD(&dev->dev_alloc_list);
	INIT_LIST_HEAD(&dev->post_commit_list);

	atomic_set(&dev->reada_in_flight, 0);
	atomic_set(&dev->dev_stats_ccnt, 0);
	btrfs_device_data_ordered_init(dev);
	INIT_RADIX_TREE(&dev->reada_zones, GFP_NOFS & ~__GFP_DIRECT_RECLAIM);
	INIT_RADIX_TREE(&dev->reada_extents, GFP_NOFS & ~__GFP_DIRECT_RECLAIM);
	extent_io_tree_init(fs_info, &dev->alloc_state,
			    IO_TREE_DEVICE_ALLOC_STATE, NULL);

	if (devid)
		tmp = *devid;
	else {
		int ret;

		ret = find_next_devid(fs_info, &tmp);
		if (ret) {
			btrfs_free_device(dev);
			return ERR_PTR(ret);
		}
	}
	dev->devid = tmp;

	if (uuid)
		memcpy(dev->uuid, uuid, BTRFS_UUID_SIZE);
	else
		generate_random_uuid(dev->uuid);

	return dev;
}