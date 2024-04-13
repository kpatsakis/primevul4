static int ext4_add_entry(handle_t *handle, struct dentry *dentry,
			  struct inode *inode)
{
	struct inode *dir = dentry->d_parent->d_inode;
	struct buffer_head *bh;
	struct ext4_dir_entry_2 *de;
	struct ext4_dir_entry_tail *t;
	struct super_block *sb;
	int	retval;
	int	dx_fallback=0;
	unsigned blocksize;
	ext4_lblk_t block, blocks;
	int	csum_size = 0;

	if (EXT4_HAS_RO_COMPAT_FEATURE(inode->i_sb,
				       EXT4_FEATURE_RO_COMPAT_METADATA_CSUM))
		csum_size = sizeof(struct ext4_dir_entry_tail);

	sb = dir->i_sb;
	blocksize = sb->s_blocksize;
	if (!dentry->d_name.len)
		return -EINVAL;

	if (ext4_has_inline_data(dir)) {
		retval = ext4_try_add_inline_entry(handle, dentry, inode);
		if (retval < 0)
			return retval;
		if (retval == 1) {
			retval = 0;
			return retval;
		}
	}

	if (is_dx(dir)) {
		retval = ext4_dx_add_entry(handle, dentry, inode);
		if (!retval || (retval != ERR_BAD_DX_DIR))
			return retval;
		ext4_clear_inode_flag(dir, EXT4_INODE_INDEX);
		dx_fallback++;
		ext4_mark_inode_dirty(handle, dir);
	}
	blocks = dir->i_size >> sb->s_blocksize_bits;
	for (block = 0; block < blocks; block++) {
		if (!(bh = ext4_bread(handle, dir, block, 0, &retval))) {
			if (!retval) {
				retval = -EIO;
				ext4_error(inode->i_sb,
					   "Directory hole detected on inode %lu\n",
					   inode->i_ino);
			}
			return retval;
		}
		if (!buffer_verified(bh) &&
		    !ext4_dirent_csum_verify(dir,
				(struct ext4_dir_entry *)bh->b_data))
			return -EIO;
		set_buffer_verified(bh);
		retval = add_dirent_to_buf(handle, dentry, inode, NULL, bh);
		if (retval != -ENOSPC) {
			brelse(bh);
			return retval;
		}

		if (blocks == 1 && !dx_fallback &&
		    EXT4_HAS_COMPAT_FEATURE(sb, EXT4_FEATURE_COMPAT_DIR_INDEX))
			return make_indexed_dir(handle, dentry, inode, bh);
		brelse(bh);
	}
	bh = ext4_append(handle, dir, &block, &retval);
	if (!bh)
		return retval;
	de = (struct ext4_dir_entry_2 *) bh->b_data;
	de->inode = 0;
	de->rec_len = ext4_rec_len_to_disk(blocksize - csum_size, blocksize);

	if (csum_size) {
		t = EXT4_DIRENT_TAIL(bh->b_data, blocksize);
		initialize_dirent_tail(t, blocksize);
	}

	retval = add_dirent_to_buf(handle, dentry, inode, de, bh);
	brelse(bh);
	if (retval == 0)
		ext4_set_inode_state(inode, EXT4_STATE_NEWENTRY);
	return retval;
}
