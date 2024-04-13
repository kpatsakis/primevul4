int ext4_handle_dirty_dirent_node(handle_t *handle,
				  struct inode *inode,
				  struct buffer_head *bh)
{
	ext4_dirent_csum_set(inode, (struct ext4_dir_entry *)bh->b_data);
	return ext4_handle_dirty_metadata(handle, inode, bh);
}
