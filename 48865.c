int jfs_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	struct inode *inode = file->f_mapping->host;
	int rc = 0;

	rc = filemap_write_and_wait_range(inode->i_mapping, start, end);
	if (rc)
		return rc;

	mutex_lock(&inode->i_mutex);
	if (!(inode->i_state & I_DIRTY) ||
	    (datasync && !(inode->i_state & I_DIRTY_DATASYNC))) {
		/* Make sure committed changes hit the disk */
		jfs_flush_journal(JFS_SBI(inode->i_sb)->log, 1);
		mutex_unlock(&inode->i_mutex);
		return rc;
	}

	rc |= jfs_commit_inode(inode, 1);
	mutex_unlock(&inode->i_mutex);

	return rc ? -EIO : 0;
}
