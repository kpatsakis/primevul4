ext4_access_path(handle_t *handle, struct inode *inode,
		struct ext4_ext_path *path)
{
	int credits, err;

	if (!ext4_handle_valid(handle))
		return 0;

	/*
	 * Check if need to extend journal credits
	 * 3 for leaf, sb, and inode plus 2 (bmap and group
	 * descriptor) for each block group; assume two block
	 * groups
	 */
	if (handle->h_buffer_credits < 7) {
		credits = ext4_writepage_trans_blocks(inode);
		err = ext4_ext_truncate_extend_restart(handle, inode, credits);
		/* EAGAIN is success */
		if (err && err != -EAGAIN)
			return err;
	}

	err = ext4_ext_get_access(handle, inode, path);
	return err;
}
