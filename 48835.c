static int expand_inode_data(struct inode *inode, loff_t offset,
					loff_t len, int mode)
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	pgoff_t index, pg_start, pg_end;
	loff_t new_size = i_size_read(inode);
	loff_t off_start, off_end;
	int ret = 0;

	ret = inode_newsize_ok(inode, (len + offset));
	if (ret)
		return ret;

	ret = f2fs_convert_inline_data(inode, offset + len);
	if (ret)
		return ret;

	pg_start = ((unsigned long long) offset) >> PAGE_CACHE_SHIFT;
	pg_end = ((unsigned long long) offset + len) >> PAGE_CACHE_SHIFT;

	off_start = offset & (PAGE_CACHE_SIZE - 1);
	off_end = (offset + len) & (PAGE_CACHE_SIZE - 1);

	for (index = pg_start; index <= pg_end; index++) {
		struct dnode_of_data dn;

		f2fs_lock_op(sbi);
		set_new_dnode(&dn, inode, NULL, NULL, 0);
		ret = f2fs_reserve_block(&dn, index);
		f2fs_unlock_op(sbi);
		if (ret)
			break;

		if (pg_start == pg_end)
			new_size = offset + len;
		else if (index == pg_start && off_start)
			new_size = (index + 1) << PAGE_CACHE_SHIFT;
		else if (index == pg_end)
			new_size = (index << PAGE_CACHE_SHIFT) + off_end;
		else
			new_size += PAGE_CACHE_SIZE;
	}

	if (!(mode & FALLOC_FL_KEEP_SIZE) &&
		i_size_read(inode) < new_size) {
		i_size_write(inode, new_size);
		mark_inode_dirty(inode);
	}

	return ret;
}
