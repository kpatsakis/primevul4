int btrfs_init_new_device(struct btrfs_fs_info *fs_info, const char *device_path)
{
	struct btrfs_root *root = fs_info->dev_root;
	struct request_queue *q;
	struct btrfs_trans_handle *trans;
	struct btrfs_device *device;
	struct block_device *bdev;
	struct super_block *sb = fs_info->sb;
	struct rcu_string *name;
	struct btrfs_fs_devices *fs_devices = fs_info->fs_devices;
	u64 orig_super_total_bytes;
	u64 orig_super_num_devices;
	int seeding_dev = 0;
	int ret = 0;
	bool locked = false;

	if (sb_rdonly(sb) && !fs_devices->seeding)
		return -EROFS;

	bdev = blkdev_get_by_path(device_path, FMODE_WRITE | FMODE_EXCL,
				  fs_info->bdev_holder);
	if (IS_ERR(bdev))
		return PTR_ERR(bdev);

	if (!btrfs_check_device_zone_type(fs_info, bdev)) {
		ret = -EINVAL;
		goto error;
	}

	if (fs_devices->seeding) {
		seeding_dev = 1;
		down_write(&sb->s_umount);
		mutex_lock(&uuid_mutex);
		locked = true;
	}

	sync_blockdev(bdev);

	rcu_read_lock();
	list_for_each_entry_rcu(device, &fs_devices->devices, dev_list) {
		if (device->bdev == bdev) {
			ret = -EEXIST;
			rcu_read_unlock();
			goto error;
		}
	}
	rcu_read_unlock();

	device = btrfs_alloc_device(fs_info, NULL, NULL);
	if (IS_ERR(device)) {
		/* we can safely leave the fs_devices entry around */
		ret = PTR_ERR(device);
		goto error;
	}

	name = rcu_string_strdup(device_path, GFP_KERNEL);
	if (!name) {
		ret = -ENOMEM;
		goto error_free_device;
	}
	rcu_assign_pointer(device->name, name);

	device->fs_info = fs_info;
	device->bdev = bdev;

	ret = btrfs_get_dev_zone_info(device);
	if (ret)
		goto error_free_device;

	trans = btrfs_start_transaction(root, 0);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		goto error_free_zone;
	}

	q = bdev_get_queue(bdev);
	set_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state);
	device->generation = trans->transid;
	device->io_width = fs_info->sectorsize;
	device->io_align = fs_info->sectorsize;
	device->sector_size = fs_info->sectorsize;
	device->total_bytes = round_down(i_size_read(bdev->bd_inode),
					 fs_info->sectorsize);
	device->disk_total_bytes = device->total_bytes;
	device->commit_total_bytes = device->total_bytes;
	set_bit(BTRFS_DEV_STATE_IN_FS_METADATA, &device->dev_state);
	clear_bit(BTRFS_DEV_STATE_REPLACE_TGT, &device->dev_state);
	device->mode = FMODE_EXCL;
	device->dev_stats_valid = 1;
	set_blocksize(device->bdev, BTRFS_BDEV_BLOCKSIZE);

	if (seeding_dev) {
		btrfs_clear_sb_rdonly(sb);
		ret = btrfs_prepare_sprout(fs_info);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto error_trans;
		}
	}

	device->fs_devices = fs_devices;

	mutex_lock(&fs_devices->device_list_mutex);
	mutex_lock(&fs_info->chunk_mutex);
	list_add_rcu(&device->dev_list, &fs_devices->devices);
	list_add(&device->dev_alloc_list, &fs_devices->alloc_list);
	fs_devices->num_devices++;
	fs_devices->open_devices++;
	fs_devices->rw_devices++;
	fs_devices->total_devices++;
	fs_devices->total_rw_bytes += device->total_bytes;

	atomic64_add(device->total_bytes, &fs_info->free_chunk_space);

	if (!blk_queue_nonrot(q))
		fs_devices->rotating = true;

	orig_super_total_bytes = btrfs_super_total_bytes(fs_info->super_copy);
	btrfs_set_super_total_bytes(fs_info->super_copy,
		round_down(orig_super_total_bytes + device->total_bytes,
			   fs_info->sectorsize));

	orig_super_num_devices = btrfs_super_num_devices(fs_info->super_copy);
	btrfs_set_super_num_devices(fs_info->super_copy,
				    orig_super_num_devices + 1);

	/*
	 * we've got more storage, clear any full flags on the space
	 * infos
	 */
	btrfs_clear_space_info_full(fs_info);

	mutex_unlock(&fs_info->chunk_mutex);

	/* Add sysfs device entry */
	btrfs_sysfs_add_device(device);

	mutex_unlock(&fs_devices->device_list_mutex);

	if (seeding_dev) {
		mutex_lock(&fs_info->chunk_mutex);
		ret = init_first_rw_device(trans);
		mutex_unlock(&fs_info->chunk_mutex);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto error_sysfs;
		}
	}

	ret = btrfs_add_dev_item(trans, device);
	if (ret) {
		btrfs_abort_transaction(trans, ret);
		goto error_sysfs;
	}

	if (seeding_dev) {
		ret = btrfs_finish_sprout(trans);
		if (ret) {
			btrfs_abort_transaction(trans, ret);
			goto error_sysfs;
		}

		/*
		 * fs_devices now represents the newly sprouted filesystem and
		 * its fsid has been changed by btrfs_prepare_sprout
		 */
		btrfs_sysfs_update_sprout_fsid(fs_devices);
	}

	ret = btrfs_commit_transaction(trans);

	if (seeding_dev) {
		mutex_unlock(&uuid_mutex);
		up_write(&sb->s_umount);
		locked = false;

		if (ret) /* transaction commit */
			return ret;

		ret = btrfs_relocate_sys_chunks(fs_info);
		if (ret < 0)
			btrfs_handle_fs_error(fs_info, ret,
				    "Failed to relocate sys chunks after device initialization. This can be fixed using the \"btrfs balance\" command.");
		trans = btrfs_attach_transaction(root);
		if (IS_ERR(trans)) {
			if (PTR_ERR(trans) == -ENOENT)
				return 0;
			ret = PTR_ERR(trans);
			trans = NULL;
			goto error_sysfs;
		}
		ret = btrfs_commit_transaction(trans);
	}

	/*
	 * Now that we have written a new super block to this device, check all
	 * other fs_devices list if device_path alienates any other scanned
	 * device.
	 * We can ignore the return value as it typically returns -EINVAL and
	 * only succeeds if the device was an alien.
	 */
	btrfs_forget_devices(device_path);

	/* Update ctime/mtime for blkid or udev */
	update_dev_time(device_path);

	return ret;

error_sysfs:
	btrfs_sysfs_remove_device(device);
	mutex_lock(&fs_info->fs_devices->device_list_mutex);
	mutex_lock(&fs_info->chunk_mutex);
	list_del_rcu(&device->dev_list);
	list_del(&device->dev_alloc_list);
	fs_info->fs_devices->num_devices--;
	fs_info->fs_devices->open_devices--;
	fs_info->fs_devices->rw_devices--;
	fs_info->fs_devices->total_devices--;
	fs_info->fs_devices->total_rw_bytes -= device->total_bytes;
	atomic64_sub(device->total_bytes, &fs_info->free_chunk_space);
	btrfs_set_super_total_bytes(fs_info->super_copy,
				    orig_super_total_bytes);
	btrfs_set_super_num_devices(fs_info->super_copy,
				    orig_super_num_devices);
	mutex_unlock(&fs_info->chunk_mutex);
	mutex_unlock(&fs_info->fs_devices->device_list_mutex);
error_trans:
	if (seeding_dev)
		btrfs_set_sb_rdonly(sb);
	if (trans)
		btrfs_end_transaction(trans);
error_free_zone:
	btrfs_destroy_dev_zone_info(device);
error_free_device:
	btrfs_free_device(device);
error:
	blkdev_put(bdev, FMODE_EXCL);
	if (locked) {
		mutex_unlock(&uuid_mutex);
		up_write(&sb->s_umount);
	}
	return ret;
}