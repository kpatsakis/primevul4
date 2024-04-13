static struct btrfs_trans_handle *__unlink_start_trans(struct inode *dir)
{
	struct btrfs_trans_handle *trans;
	struct btrfs_root *root = BTRFS_I(dir)->root;
	int ret;

	/*
	 * 1 for the possible orphan item
	 * 1 for the dir item
	 * 1 for the dir index
	 * 1 for the inode ref
	 * 1 for the inode
	 */
	trans = btrfs_start_transaction(root, 5);
	if (!IS_ERR(trans) || PTR_ERR(trans) != -ENOSPC)
		return trans;

	if (PTR_ERR(trans) == -ENOSPC) {
		u64 num_bytes = btrfs_calc_trans_metadata_size(root, 5);

		trans = btrfs_start_transaction(root, 0);
		if (IS_ERR(trans))
			return trans;
		ret = btrfs_cond_migrate_bytes(root->fs_info,
					       &root->fs_info->trans_block_rsv,
					       num_bytes, 5);
		if (ret) {
			btrfs_end_transaction(trans, root);
			return ERR_PTR(ret);
		}
		trans->block_rsv = &root->fs_info->trans_block_rsv;
		trans->bytes_reserved = num_bytes;
	}
	return trans;
}
