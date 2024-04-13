int ubifs_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	struct inode *inode = file->f_mapping->host;
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	int err;

	dbg_gen("syncing inode %lu", inode->i_ino);

	if (c->ro_mount)
		/*
		 * For some really strange reasons VFS does not filter out
		 * 'fsync()' for R/O mounted file-systems as per 2.6.39.
		 */
		return 0;

	err = filemap_write_and_wait_range(inode->i_mapping, start, end);
	if (err)
		return err;
	mutex_lock(&inode->i_mutex);

	/* Synchronize the inode unless this is a 'datasync()' call. */
	if (!datasync || (inode->i_state & I_DIRTY_DATASYNC)) {
		err = inode->i_sb->s_op->write_inode(inode, NULL);
		if (err)
			goto out;
	}

	/*
	 * Nodes related to this inode may still sit in a write-buffer. Flush
	 * them.
	 */
	err = ubifs_sync_wbufs_by_inode(c, inode);
out:
	mutex_unlock(&inode->i_mutex);
	return err;
}
