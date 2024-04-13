int btrfs_relocate_chunk(struct btrfs_fs_info *fs_info, u64 chunk_offset)
{
	struct btrfs_root *root = fs_info->chunk_root;
	struct btrfs_trans_handle *trans;
	struct btrfs_block_group *block_group;
	u64 length;
	int ret;

	/*
	 * Prevent races with automatic removal of unused block groups.
	 * After we relocate and before we remove the chunk with offset
	 * chunk_offset, automatic removal of the block group can kick in,
	 * resulting in a failure when calling btrfs_remove_chunk() below.
	 *
	 * Make sure to acquire this mutex before doing a tree search (dev
	 * or chunk trees) to find chunks. Otherwise the cleaner kthread might
	 * call btrfs_remove_chunk() (through btrfs_delete_unused_bgs()) after
	 * we release the path used to search the chunk/dev tree and before
	 * the current task acquires this mutex and calls us.
	 */
	lockdep_assert_held(&fs_info->reclaim_bgs_lock);

	/* step one, relocate all the extents inside this chunk */
	btrfs_scrub_pause(fs_info);
	ret = btrfs_relocate_block_group(fs_info, chunk_offset);
	btrfs_scrub_continue(fs_info);
	if (ret)
		return ret;

	block_group = btrfs_lookup_block_group(fs_info, chunk_offset);
	if (!block_group)
		return -ENOENT;
	btrfs_discard_cancel_work(&fs_info->discard_ctl, block_group);
	length = block_group->length;
	btrfs_put_block_group(block_group);

	/*
	 * On a zoned file system, discard the whole block group, this will
	 * trigger a REQ_OP_ZONE_RESET operation on the device zone. If
	 * resetting the zone fails, don't treat it as a fatal problem from the
	 * filesystem's point of view.
	 */
	if (btrfs_is_zoned(fs_info)) {
		ret = btrfs_discard_extent(fs_info, chunk_offset, length, NULL);
		if (ret)
			btrfs_info(fs_info,
				"failed to reset zone %llu after relocation",
				chunk_offset);
	}

	trans = btrfs_start_trans_remove_block_group(root->fs_info,
						     chunk_offset);
	if (IS_ERR(trans)) {
		ret = PTR_ERR(trans);
		btrfs_handle_fs_error(root->fs_info, ret, NULL);
		return ret;
	}

	/*
	 * step two, delete the device extents and the
	 * chunk tree entries
	 */
	ret = btrfs_remove_chunk(trans, chunk_offset);
	btrfs_end_transaction(trans);
	return ret;
}