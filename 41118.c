static int ext4_unlink(struct inode *dir, struct dentry *dentry)
{
	int retval;
	struct inode *inode;
	struct buffer_head *bh;
	struct ext4_dir_entry_2 *de;
	handle_t *handle;

	trace_ext4_unlink_enter(dir, dentry);
	/* Initialize quotas before so that eventual writes go
	 * in separate transaction */
	dquot_initialize(dir);
	dquot_initialize(dentry->d_inode);

	handle = ext4_journal_start(dir, EXT4_DELETE_TRANS_BLOCKS(dir->i_sb));
	if (IS_ERR(handle))
		return PTR_ERR(handle);

	if (IS_DIRSYNC(dir))
		ext4_handle_sync(handle);

	retval = -ENOENT;
	bh = ext4_find_entry(dir, &dentry->d_name, &de, NULL);
	if (!bh)
		goto end_unlink;

	inode = dentry->d_inode;

	retval = -EIO;
	if (le32_to_cpu(de->inode) != inode->i_ino)
		goto end_unlink;

	if (!inode->i_nlink) {
		ext4_warning(inode->i_sb,
			     "Deleting nonexistent file (%lu), %d",
			     inode->i_ino, inode->i_nlink);
		set_nlink(inode, 1);
	}
	retval = ext4_delete_entry(handle, dir, de, bh);
	if (retval)
		goto end_unlink;
	dir->i_ctime = dir->i_mtime = ext4_current_time(dir);
	ext4_update_dx_flag(dir);
	ext4_mark_inode_dirty(handle, dir);
	drop_nlink(inode);
	if (!inode->i_nlink)
		ext4_orphan_add(handle, inode);
	inode->i_ctime = ext4_current_time(inode);
	ext4_mark_inode_dirty(handle, inode);
	retval = 0;

end_unlink:
	ext4_journal_stop(handle);
	brelse(bh);
	trace_ext4_unlink_exit(dentry, retval);
	return retval;
}
