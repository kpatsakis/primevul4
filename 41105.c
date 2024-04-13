static int ext4_init_new_dir(handle_t *handle, struct inode *dir,
			     struct inode *inode)
{
	struct buffer_head *dir_block = NULL;
	struct ext4_dir_entry_2 *de;
	struct ext4_dir_entry_tail *t;
	unsigned int blocksize = dir->i_sb->s_blocksize;
	int csum_size = 0;
	int err;

	if (EXT4_HAS_RO_COMPAT_FEATURE(dir->i_sb,
				       EXT4_FEATURE_RO_COMPAT_METADATA_CSUM))
		csum_size = sizeof(struct ext4_dir_entry_tail);

	if (ext4_test_inode_state(inode, EXT4_STATE_MAY_INLINE_DATA)) {
		err = ext4_try_create_inline_dir(handle, dir, inode);
		if (err < 0 && err != -ENOSPC)
			goto out;
		if (!err)
			goto out;
	}

	inode->i_size = EXT4_I(inode)->i_disksize = blocksize;
	dir_block = ext4_bread(handle, inode, 0, 1, &err);
	if (!(dir_block = ext4_bread(handle, inode, 0, 1, &err))) {
		if (!err) {
			err = -EIO;
			ext4_error(inode->i_sb,
				   "Directory hole detected on inode %lu\n",
				   inode->i_ino);
		}
		goto out;
	}
	BUFFER_TRACE(dir_block, "get_write_access");
	err = ext4_journal_get_write_access(handle, dir_block);
	if (err)
		goto out;
	de = (struct ext4_dir_entry_2 *)dir_block->b_data;
	ext4_init_dot_dotdot(inode, de, blocksize, csum_size, dir->i_ino, 0);
	set_nlink(inode, 2);
	if (csum_size) {
		t = EXT4_DIRENT_TAIL(dir_block->b_data, blocksize);
		initialize_dirent_tail(t, blocksize);
	}

	BUFFER_TRACE(dir_block, "call ext4_handle_dirty_metadata");
	err = ext4_handle_dirty_dirent_node(handle, inode, dir_block);
	if (err)
		goto out;
	set_buffer_verified(dir_block);
out:
	brelse(dir_block);
	return err;
}
