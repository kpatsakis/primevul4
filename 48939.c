static int update_mctime(struct inode *inode)
{
	struct timespec now = ubifs_current_time(inode);
	struct ubifs_inode *ui = ubifs_inode(inode);
	struct ubifs_info *c = inode->i_sb->s_fs_info;

	if (mctime_update_needed(inode, &now)) {
		int err, release;
		struct ubifs_budget_req req = { .dirtied_ino = 1,
				.dirtied_ino_d = ALIGN(ui->data_len, 8) };

		err = ubifs_budget_space(c, &req);
		if (err)
			return err;

		mutex_lock(&ui->ui_mutex);
		inode->i_mtime = inode->i_ctime = ubifs_current_time(inode);
		release = ui->dirty;
		mark_inode_dirty_sync(inode);
		mutex_unlock(&ui->ui_mutex);
		if (release)
			ubifs_release_budget(c, &req);
	}

	return 0;
}
