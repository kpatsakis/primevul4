int btrfs_shrink_device(struct btrfs_device *device, u64 new_size)
{
	struct btrfs_fs_info *fs_info = device->fs_info;
	struct btrfs_root *root = fs_info->dev_root;
	struct btrfs_trans_handle *trans;
	struct btrfs_dev_extent *dev_extent = NULL;
	struct btrfs_path *path;
	u64 length;
	u64 chunk_offset;
	int ret;
	int slot;
	int failed = 0;
	bool retried = false;
	struct extent_buffer *l;
	struct btrfs_key key;
	struct btrfs_super_block *super_copy = fs_info->super_copy;
	u64 old_total = btrfs_super_total_bytes(super_copy);
	u64 old_size = btrfs_device_get_total_bytes(device);
	u64 diff;
	u64 start;

	new_size = round_down(new_size, fs_info->sectorsize);
	start = new_size;
	diff = round_down(old_size - new_size, fs_info->sectorsize);

	if (test_bit(BTRFS_DEV_STATE_REPLACE_TGT, &device->dev_state))
		return -EINVAL;

	path = btrfs_alloc_path();
	if (!path)
		return -ENOMEM;

	path->reada = READA_BACK;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		btrfs_free_path(path);
		return PTR_ERR(trans);
	}

	mutex_lock(&fs_info->chunk_mutex);

	btrfs_device_set_total_bytes(device, new_size);
	if (test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state)) {
		device->fs_devices->total_rw_bytes -= diff;
		atomic64_sub(diff, &fs_info->free_chunk_space);
	}

	/*
	 * Once the device's size has been set to the new size, ensure all
	 * in-memory chunks are synced to disk so that the loop below sees them
	 * and relocates them accordingly.
	 */
	if (contains_pending_extent(device, &start, diff)) {
		mutex_unlock(&fs_info->chunk_mutex);
		ret = btrfs_commit_transaction(trans);
		if (ret)
			goto done;
	} else {
		mutex_unlock(&fs_info->chunk_mutex);
		btrfs_end_transaction(trans);
	}

again:
	key.objectid = device->devid;
	key.offset = (u64)-1;
	key.type = BTRFS_DEV_EXTENT_KEY;

	do {
		mutex_lock(&fs_info->reclaim_bgs_lock);
		ret = btrfs_search_slot(NULL, root, &key, path, 0, 0);
		if (ret < 0) {
			mutex_unlock(&fs_info->reclaim_bgs_lock);
			goto done;
		}

		ret = btrfs_previous_item(root, path, 0, key.type);
		if (ret) {
			mutex_unlock(&fs_info->reclaim_bgs_lock);
			if (ret < 0)
				goto done;
			ret = 0;
			btrfs_release_path(path);
			break;
		}

		l = path->nodes[0];
		slot = path->slots[0];
		btrfs_item_key_to_cpu(l, &key, path->slots[0]);

		if (key.objectid != device->devid) {
			mutex_unlock(&fs_info->reclaim_bgs_lock);
			btrfs_release_path(path);
			break;
		}

		dev_extent = btrfs_item_ptr(l, slot, struct btrfs_dev_extent);
		length = btrfs_dev_extent_length(l, dev_extent);

		if (key.offset + length <= new_size) {
			mutex_unlock(&fs_info->reclaim_bgs_lock);
			btrfs_release_path(path);
			break;
		}

		chunk_offset = btrfs_dev_extent_chunk_offset(l, dev_extent);
		btrfs_release_path(path);

		/*
		 * We may be relocating the only data chunk we have,
		 * which could potentially end up with losing data's
		 * raid profile, so lets allocate an empty one in
		 * advance.
		 */
		ret = btrfs_may_alloc_data_chunk(fs_info, chunk_offset);
		if (ret < 0) {
			mutex_unlock(&fs_info->reclaim_bgs_lock);
			goto done;
		}

		ret = btrfs_relocate_chunk(fs_info, chunk_offset);
		mutex_unlock(&fs_info->reclaim_bgs_lock);
		if (ret == -ENOSPC) {
			failed++;
		} else if (ret) {
			if (ret == -ETXTBSY) {
				btrfs_warn(fs_info,
		   "could not shrink block group %llu due to active swapfile",
					   chunk_offset);
			}
			goto done;
		}
	} while (key.offset-- > 0);

	if (failed && !retried) {
		failed = 0;
		retried = true;
		goto again;
	} else if (failed && retried) {
		ret = -ENOSPC;
		goto done;
	}

	/* Shrinking succeeded, else we would be at "done". */
	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto done;
	}

	mutex_lock(&fs_info->chunk_mutex);
	/* Clear all state bits beyond the shrunk device size */
	clear_extent_bits(&device->alloc_state, new_size, (u64)-1,
			  CHUNK_STATE_MASK);

	btrfs_device_set_disk_total_bytes(device, new_size);
	if (list_empty(&device->post_commit_list))
		list_add_tail(&device->post_commit_list,
			      &trans->transaction->dev_update_list);

	WARN_ON(diff > old_total);
	btrfs_set_super_total_bytes(super_copy,
			round_down(old_total - diff, fs_info->sectorsize));
	mutex_unlock(&fs_info->chunk_mutex);

	/* Now btrfs_update_device() will change the on-disk size. */
	ret = btrfs_update_device(trans, device);
	if (ret < 0) {
		btrfs_abort_transaction(trans, ret);
		btrfs_end_transaction(trans);
	} else {
		ret = btrfs_commit_transaction(trans);
	}
done:
	btrfs_free_path(path);
	if (ret) {
		mutex_lock(&fs_info->chunk_mutex);
		btrfs_device_set_total_bytes(device, old_size);
		if (test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state))
			device->fs_devices->total_rw_bytes += diff;
		atomic64_add(diff, &fs_info->free_chunk_space);
		mutex_unlock(&fs_info->chunk_mutex);
	}
	return ret;
}