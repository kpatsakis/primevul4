static int reiserfs_file_open(struct inode *inode, struct file *file)
{
	int err = dquot_file_open(inode, file);
        if (!atomic_inc_not_zero(&REISERFS_I(inode)->openers)) {
		/* somebody might be tailpacking on final close; wait for it */
		mutex_lock(&(REISERFS_I(inode)->tailpack));
		atomic_inc(&REISERFS_I(inode)->openers);
		mutex_unlock(&(REISERFS_I(inode)->tailpack));
	}
	return err;
}
