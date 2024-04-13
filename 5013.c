int btrfs_remove_chunk(struct btrfs_trans_handle *trans, u64 chunk_offset)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	struct extent_map *em;
	struct map_lookup *map;
	u64 dev_extent_len = 0;
	int i, ret = 0;
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;

	em = btrfs_get_chunk_map(fs_info, chunk_offset, 1);
	if (IS_ERR(em)) {
		/*
		 * This is a logic error, but we don't want to just rely on the
		 * user having built with ASSERT enabled, so if ASSERT doesn't
		 * do anything we still error out.
		 */
		ASSERT(0);
		return PTR_ERR(em);
	}
	map = em->map_lookup;

	/*
	 * First delete the device extent items from the devices btree.
	 * We take the device_list_mutex to avoid racing with the finishing phase
	 * of a device replace operation. See the comment below before acquiring
	 * fs_info->chunk_mutex. Note that here we do not acquire the chunk_mutex
	 * because that can result in a deadlock when deleting the device extent
	 * items from the devices btree - COWing an extent buffer from the btree
	 * may result in allocating a new metadata chunk, which would attempt to
	 * lock again fs_info->chunk_mutex.
	 */
	mutex_lock(&fs_devices->device_list_mutex);
	for (i = 0; i < map->num_stripes; i++) {
		struct btrfs_device *device = map->stripes[i].dev;
		ret = btrfs_free_dev_extent(trans, device,
					    map->stripes[i].physical,
					    &dev_extent_len);
		if (ret) {
			mutex_unlock(&fs_devices->device_list_mutex);
			btrfs_abort_transaction(trans, ret);
			goto out;
		}

		if (device->bytes_used > 0) {
			mutex_lock(&fs_info->chunk_mutex);
			btrfs_device_set_bytes_used(device,
					device->bytes_used - dev_extent_len);
			atomic64_add(dev_extent_len, &fs_info->free_chunk_space);
			btrfs_clear_space_info_full(fs_info);
			mutex_unlock(&fs_info->chunk_mutex);
		}
	}
	mutex_unlock(&fs_devices->device_list_mutex);

	/*
	 * We acquire fs_info->chunk_mutex for 2 reasons:
	 *
	 * 1) Just like with the first phase of the chunk allocation, we must
	 *    reserve system space, do all chunk btree updates and deletions, and
	 *    update the system chunk array in the superblock while holding this
	 *    mutex. This is for similar reasons as explained on the comment at
	 *    the top of btrfs_chunk_alloc();
	 *
	 * 2) Prevent races with the final phase of a device replace operation
	 *    that replaces the device object associated with the map's stripes,
	 *    because the device object's id can change at any time during that
	 *    final phase of the device replace operation
	 *    (dev-replace.c:btrfs_dev_replace_finishing()), so we could grab the
	 *    replaced device and then see it with an ID of
	 *    BTRFS_DEV_REPLACE_DEVID, which would cause a failure when updating
	 *    the device item, which does not exists on the chunk btree.
	 *    The finishing phase of device replace acquires both the
	 *    device_list_mutex and the chunk_mutex, in that order, so we are
	 *    safe by just acquiring the chunk_mutex.
	 */
	trans->removing_chunk = true;
	mutex_lock(&fs_info->chunk_mutex);

	check_system_chunk(trans, map->type);

	ret = remove_chunk_item(trans, map, chunk_offset);
	/*
	 * Normally we should not get -ENOSPC since we reserved space before
	 * through the call to check_system_chunk().
	 *
	 * Despite our system space_info having enough free space, we may not
	 * be able to allocate extents from its block groups, because all have
	 * an incompatible profile, which will force us to allocate a new system
	 * block group with the right profile, or right after we called
	 * check_system_space() above, a scrub turned the only system block group
	 * with enough free space into RO mode.
	 * This is explained with more detail at do_chunk_alloc().
	 *
	 * So if we get -ENOSPC, allocate a new system chunk and retry once.
	 */
	if (ret == -ENOSPC) {
		const u64 sys_flags = btrfs_system_alloc_profile(fs_info);
		struct btrfs_block_group *sys_bg;

		sys_bg = btrfs_alloc_chunk(trans, sys_flags);
		if (IS_ERR(sys_bg)) {
			ret = PTR_ERR(sys_bg);
			btrfs_abort_transaction(trans, ret);
			goto out;
		}

		ret = btrfs_chunk_alloc_add_chunk_item(trans, sys_bg);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto out;
		}

		ret = remove_chunk_item(trans, map, chunk_offset);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto out;
		}
	} else if (ret) {
		btrfs_abort_transaction(trans, ret);
		goto out;
	}

	trace_btrfs_chunk_free(fs_info, map, chunk_offset, em->len);

	if (map->type & BTRFS_BLOCK_GROUP_SYSTEM) {
		ret = btrfs_del_sys_chunk(fs_info, chunk_offset);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto out;
		}
	}

	mutex_unlock(&fs_info->chunk_mutex);
	trans->removing_chunk = false;

	/*
	 * We are done with chunk btree updates and deletions, so release the
	 * system space we previously reserved (with check_system_chunk()).
	 */
	btrfs_trans_release_chunk_metadata(trans);

	ret = btrfs_remove_block_group(trans, chunk_offset, em);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		goto out;
	}

out:
	if (trans->removing_chunk) {
		mutex_unlock(&fs_info->chunk_mutex);
		trans->removing_chunk = false;
	}
	/* once for us */
	free_extent_map(em);
	return ret;
}