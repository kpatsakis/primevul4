int f2fs_sync_file(struct file *file, loff_t start, loff_t end, int datasync)
{
	struct inode *inode = file->f_mapping->host;
	struct f2fs_inode_info *fi = F2FS_I(inode);
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	int ret = 0;
	bool need_cp = false;
	struct writeback_control wbc = {
		.sync_mode = WB_SYNC_ALL,
		.nr_to_write = LONG_MAX,
		.for_reclaim = 0,
	};

	if (unlikely(f2fs_readonly(inode->i_sb)))
		return 0;

	trace_f2fs_sync_file_enter(inode);
	ret = filemap_write_and_wait_range(inode->i_mapping, start, end);
	if (ret) {
		trace_f2fs_sync_file_exit(inode, need_cp, datasync, ret);
		return ret;
	}

	/* guarantee free sections for fsync */
	f2fs_balance_fs(sbi);

	down_read(&fi->i_sem);

	/*
	 * Both of fdatasync() and fsync() are able to be recovered from
	 * sudden-power-off.
	 */
	if (!S_ISREG(inode->i_mode) || inode->i_nlink != 1)
		need_cp = true;
	else if (file_wrong_pino(inode))
		need_cp = true;
	else if (!space_for_roll_forward(sbi))
		need_cp = true;
	else if (!is_checkpointed_node(sbi, F2FS_I(inode)->i_pino))
		need_cp = true;
	else if (F2FS_I(inode)->xattr_ver == cur_cp_version(F2FS_CKPT(sbi)))
		need_cp = true;

	up_read(&fi->i_sem);

	if (need_cp) {
		nid_t pino;

		/* all the dirty node pages should be flushed for POR */
		ret = f2fs_sync_fs(inode->i_sb, 1);

		down_write(&fi->i_sem);
		F2FS_I(inode)->xattr_ver = 0;
		if (file_wrong_pino(inode) && inode->i_nlink == 1 &&
					get_parent_ino(inode, &pino)) {
			F2FS_I(inode)->i_pino = pino;
			file_got_pino(inode);
			up_write(&fi->i_sem);
			mark_inode_dirty_sync(inode);
			ret = f2fs_write_inode(inode, NULL);
			if (ret)
				goto out;
		} else {
			up_write(&fi->i_sem);
		}
	} else {
		/* if there is no written node page, write its inode page */
		while (!sync_node_pages(sbi, inode->i_ino, &wbc)) {
			if (fsync_mark_done(sbi, inode->i_ino))
				goto out;
			mark_inode_dirty_sync(inode);
			ret = f2fs_write_inode(inode, NULL);
			if (ret)
				goto out;
		}
		ret = wait_on_node_pages_writeback(sbi, inode->i_ino);
		if (ret)
			goto out;
		ret = f2fs_issue_flush(F2FS_SB(inode->i_sb));
	}
out:
	trace_f2fs_sync_file_exit(inode, need_cp, datasync, ret);
	return ret;
}
