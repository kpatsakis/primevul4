static int do_setattr(struct ubifs_info *c, struct inode *inode,
		      const struct iattr *attr)
{
	int err, release;
	loff_t new_size = attr->ia_size;
	struct ubifs_inode *ui = ubifs_inode(inode);
	struct ubifs_budget_req req = { .dirtied_ino = 1,
				.dirtied_ino_d = ALIGN(ui->data_len, 8) };

	err = ubifs_budget_space(c, &req);
	if (err)
		return err;

	if (attr->ia_valid & ATTR_SIZE) {
		dbg_gen("size %lld -> %lld", inode->i_size, new_size);
		truncate_setsize(inode, new_size);
	}

	mutex_lock(&ui->ui_mutex);
	if (attr->ia_valid & ATTR_SIZE) {
		/* Truncation changes inode [mc]time */
		inode->i_mtime = inode->i_ctime = ubifs_current_time(inode);
		/* 'truncate_setsize()' changed @i_size, update @ui_size */
		ui->ui_size = inode->i_size;
	}

	do_attr_changes(inode, attr);

	release = ui->dirty;
	if (attr->ia_valid & ATTR_SIZE)
		/*
		 * Inode length changed, so we have to make sure
		 * @I_DIRTY_DATASYNC is set.
		 */
		 __mark_inode_dirty(inode, I_DIRTY_SYNC | I_DIRTY_DATASYNC);
	else
		mark_inode_dirty_sync(inode);
	mutex_unlock(&ui->ui_mutex);

	if (release)
		ubifs_release_budget(c, &req);
	if (IS_SYNC(inode))
		err = inode->i_sb->s_op->write_inode(inode, NULL);
	return err;
}
