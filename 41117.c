static int ext4_symlink(struct inode *dir,
			struct dentry *dentry, const char *symname)
{
	handle_t *handle;
	struct inode *inode;
	int l, err, retries = 0;
	int credits;

	l = strlen(symname)+1;
	if (l > dir->i_sb->s_blocksize)
		return -ENAMETOOLONG;

	dquot_initialize(dir);

	if (l > EXT4_N_BLOCKS * 4) {
		/*
		 * For non-fast symlinks, we just allocate inode and put it on
		 * orphan list in the first transaction => we need bitmap,
		 * group descriptor, sb, inode block, quota blocks, and
		 * possibly selinux xattr blocks.
		 */
		credits = 4 + EXT4_MAXQUOTAS_INIT_BLOCKS(dir->i_sb) +
			  EXT4_XATTR_TRANS_BLOCKS;
	} else {
		/*
		 * Fast symlink. We have to add entry to directory
		 * (EXT4_DATA_TRANS_BLOCKS + EXT4_INDEX_EXTRA_TRANS_BLOCKS),
		 * allocate new inode (bitmap, group descriptor, inode block,
		 * quota blocks, sb is already counted in previous macros).
		 */
		credits = EXT4_DATA_TRANS_BLOCKS(dir->i_sb) +
			  EXT4_INDEX_EXTRA_TRANS_BLOCKS + 3 +
			  EXT4_MAXQUOTAS_INIT_BLOCKS(dir->i_sb);
	}
retry:
	handle = ext4_journal_start(dir, credits);
	if (IS_ERR(handle))
		return PTR_ERR(handle);

	if (IS_DIRSYNC(dir))
		ext4_handle_sync(handle);

	inode = ext4_new_inode(handle, dir, S_IFLNK|S_IRWXUGO,
			       &dentry->d_name, 0, NULL);
	err = PTR_ERR(inode);
	if (IS_ERR(inode))
		goto out_stop;

	if (l > EXT4_N_BLOCKS * 4) {
		inode->i_op = &ext4_symlink_inode_operations;
		ext4_set_aops(inode);
		/*
		 * We cannot call page_symlink() with transaction started
		 * because it calls into ext4_write_begin() which can wait
		 * for transaction commit if we are running out of space
		 * and thus we deadlock. So we have to stop transaction now
		 * and restart it when symlink contents is written.
		 * 
		 * To keep fs consistent in case of crash, we have to put inode
		 * to orphan list in the mean time.
		 */
		drop_nlink(inode);
		err = ext4_orphan_add(handle, inode);
		ext4_journal_stop(handle);
		if (err)
			goto err_drop_inode;
		err = __page_symlink(inode, symname, l, 1);
		if (err)
			goto err_drop_inode;
		/*
		 * Now inode is being linked into dir (EXT4_DATA_TRANS_BLOCKS
		 * + EXT4_INDEX_EXTRA_TRANS_BLOCKS), inode is also modified
		 */
		handle = ext4_journal_start(dir,
				EXT4_DATA_TRANS_BLOCKS(dir->i_sb) +
				EXT4_INDEX_EXTRA_TRANS_BLOCKS + 1);
		if (IS_ERR(handle)) {
			err = PTR_ERR(handle);
			goto err_drop_inode;
		}
		set_nlink(inode, 1);
		err = ext4_orphan_del(handle, inode);
		if (err) {
			ext4_journal_stop(handle);
			clear_nlink(inode);
			goto err_drop_inode;
		}
	} else {
		/* clear the extent format for fast symlink */
		ext4_clear_inode_flag(inode, EXT4_INODE_EXTENTS);
		inode->i_op = &ext4_fast_symlink_inode_operations;
		memcpy((char *)&EXT4_I(inode)->i_data, symname, l);
		inode->i_size = l-1;
	}
	EXT4_I(inode)->i_disksize = inode->i_size;
	err = ext4_add_nondir(handle, dentry, inode);
out_stop:
	ext4_journal_stop(handle);
	if (err == -ENOSPC && ext4_should_retry_alloc(dir->i_sb, &retries))
		goto retry;
	return err;
err_drop_inode:
	unlock_new_inode(inode);
	iput(inode);
	return err;
}
