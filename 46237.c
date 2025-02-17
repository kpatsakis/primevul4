static int truncate_space_check(struct btrfs_trans_handle *trans,
				struct btrfs_root *root,
				u64 bytes_deleted)
{
	int ret;

	bytes_deleted = btrfs_csum_bytes_to_leaves(root, bytes_deleted);
	ret = btrfs_block_rsv_add(root, &root->fs_info->trans_block_rsv,
				  bytes_deleted, BTRFS_RESERVE_NO_FLUSH);
	if (!ret)
		trans->bytes_reserved += bytes_deleted;
	return ret;
 
 }
