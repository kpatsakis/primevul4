static struct buffer_head *ext4_append(handle_t *handle,
					struct inode *inode,
					ext4_lblk_t *block, int *err)
{
	struct buffer_head *bh;

	if (unlikely(EXT4_SB(inode->i_sb)->s_max_dir_size_kb &&
		     ((inode->i_size >> 10) >=
		      EXT4_SB(inode->i_sb)->s_max_dir_size_kb))) {
		*err = -ENOSPC;
		return NULL;
	}

	*block = inode->i_size >> inode->i_sb->s_blocksize_bits;

	bh = ext4_bread(handle, inode, *block, 1, err);
	if (bh) {
		inode->i_size += inode->i_sb->s_blocksize;
		EXT4_I(inode)->i_disksize = inode->i_size;
		*err = ext4_journal_get_write_access(handle, bh);
		if (*err) {
			brelse(bh);
			bh = NULL;
		}
	}
	if (!bh && !(*err)) {
		*err = -EIO;
		ext4_error(inode->i_sb,
			   "Directory hole detected on inode %lu\n",
			   inode->i_ino);
	}
	return bh;
}
