static int ext4_delete_entry(handle_t *handle,
			     struct inode *dir,
			     struct ext4_dir_entry_2 *de_del,
			     struct buffer_head *bh)
{
	int err, csum_size = 0;

	if (ext4_has_inline_data(dir)) {
		int has_inline_data = 1;
		err = ext4_delete_inline_entry(handle, dir, de_del, bh,
					       &has_inline_data);
		if (has_inline_data)
			return err;
	}

	if (EXT4_HAS_RO_COMPAT_FEATURE(dir->i_sb,
				       EXT4_FEATURE_RO_COMPAT_METADATA_CSUM))
		csum_size = sizeof(struct ext4_dir_entry_tail);

	BUFFER_TRACE(bh, "get_write_access");
	err = ext4_journal_get_write_access(handle, bh);
	if (unlikely(err))
		goto out;

	err = ext4_generic_delete_entry(handle, dir, de_del,
					bh, bh->b_data,
					dir->i_sb->s_blocksize, csum_size);
	if (err)
		goto out;

	BUFFER_TRACE(bh, "call ext4_handle_dirty_metadata");
	err = ext4_handle_dirty_dirent_node(handle, dir, bh);
	if (unlikely(err))
		goto out;

	return 0;
out:
	if (err != -ENOENT)
		ext4_std_error(dir->i_sb, err);
	return err;
}
