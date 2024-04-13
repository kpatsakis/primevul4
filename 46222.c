static int maybe_insert_hole(struct btrfs_root *root, struct inode *inode,
			     u64 offset, u64 len)
{
	struct btrfs_trans_handle *trans;
	int ret;

	/*
	 * Still need to make sure the inode looks like it's been updated so
	 * that any holes get logged if we fsync.
	 */
	if (btrfs_fs_incompat(root->fs_info, NO_HOLES)) {
		BTRFS_I(inode)->last_trans = root->fs_info->generation;
		BTRFS_I(inode)->last_sub_trans = root->log_transid;
		BTRFS_I(inode)->last_log_commit = root->last_log_commit;
		return 0;
	}

	/*
	 * 1 - for the one we're dropping
	 * 1 - for the one we're adding
	 * 1 - for updating the inode.
	 */
	trans = btrfs_start_transaction(root, 3);
	if (IS_ERR(trans))
		return PTR_ERR(trans);

	ret = btrfs_drop_extents(trans, root, inode, offset, offset + len, 1);
	if (ret) {
		btrfs_abort_transaction(trans, root, ret);
		btrfs_end_transaction(trans, root);
		return ret;
	}

	ret = btrfs_insert_file_extent(trans, root, btrfs_ino(inode), offset,
				       0, 0, len, 0, len, 0, 0, 0);
	if (ret)
		btrfs_abort_transaction(trans, root, ret);
	else
		btrfs_update_inode(trans, root, inode);
	btrfs_end_transaction(trans, root);
	return ret;
}
