struct btrfs_block_group *btrfs_alloc_chunk(struct btrfs_trans_handle *trans,
					    u64 type)
{
	struct btrfs_fs_info *info = trans->fs_info;
	struct btrfs_fs_devices *fs_devices = info->fs_devices;
	struct btrfs_device_info *devices_info = NULL;
	struct alloc_chunk_ctl ctl;
	struct btrfs_block_group *block_group;
	int ret;

	lockdep_assert_held(&info->chunk_mutex);

	if (!alloc_profile_is_valid(type, 0)) {
		ASSERT(0);
		return ERR_PTR(-EINVAL);
	}

	if (list_empty(&fs_devices->alloc_list)) {
		if (btrfs_test_opt(info, ENOSPC_DEBUG))
			btrfs_debug(info, "%s: no writable device", __func__);
		return ERR_PTR(-ENOSPC);
	}

	if (!(type & BTRFS_BLOCK_GROUP_TYPE_MASK)) {
		btrfs_err(info, "invalid chunk type 0x%llx requested", type);
		ASSERT(0);
		return ERR_PTR(-EINVAL);
	}

	ctl.start = find_next_chunk(info);
	ctl.type = type;
	init_alloc_chunk_ctl(fs_devices, &ctl);

	devices_info = kcalloc(fs_devices->rw_devices, sizeof(*devices_info),
			       GFP_NOFS);
	if (!devices_info)
		return ERR_PTR(-ENOMEM);

	ret = gather_device_info(fs_devices, &ctl, devices_info);
	if (ret < 0) {
		block_group = ERR_PTR(ret);
		goto out;
	}

	ret = decide_stripe_size(fs_devices, &ctl, devices_info);
	if (ret < 0) {
		block_group = ERR_PTR(ret);
		goto out;
	}

	block_group = create_chunk(trans, &ctl, devices_info);

out:
	kfree(devices_info);
	return block_group;
}