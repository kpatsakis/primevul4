static noinline int init_first_rw_device(struct btrfs_trans_handle *trans)
{
	struct btrfs_fs_info *fs_info = trans->fs_info;
	u64 alloc_profile;
	struct btrfs_block_group *meta_bg;
	struct btrfs_block_group *sys_bg;

	/*
	 * When adding a new device for sprouting, the seed device is read-only
	 * so we must first allocate a metadata and a system chunk. But before
	 * adding the block group items to the extent, device and chunk btrees,
	 * we must first:
	 *
	 * 1) Create both chunks without doing any changes to the btrees, as
	 *    otherwise we would get -ENOSPC since the block groups from the
	 *    seed device are read-only;
	 *
	 * 2) Add the device item for the new sprout device - finishing the setup
	 *    of a new block group requires updating the device item in the chunk
	 *    btree, so it must exist when we attempt to do it. The previous step
	 *    ensures this does not fail with -ENOSPC.
	 *
	 * After that we can add the block group items to their btrees:
	 * update existing device item in the chunk btree, add a new block group
	 * item to the extent btree, add a new chunk item to the chunk btree and
	 * finally add the new device extent items to the devices btree.
	 */

	alloc_profile = btrfs_metadata_alloc_profile(fs_info);
	meta_bg = btrfs_alloc_chunk(trans, alloc_profile);
	if (IS_ERR(meta_bg))
		return PTR_ERR(meta_bg);

	alloc_profile = btrfs_system_alloc_profile(fs_info);
	sys_bg = btrfs_alloc_chunk(trans, alloc_profile);
	if (IS_ERR(sys_bg))
		return PTR_ERR(sys_bg);

	return 0;
}