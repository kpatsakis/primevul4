int fuse_open_common(struct inode *inode, struct file *file, bool isdir)
{
	struct fuse_conn *fc = get_fuse_conn(inode);
	int err;
	bool lock_inode = (file->f_flags & O_TRUNC) &&
			  fc->atomic_o_trunc &&
			  fc->writeback_cache;

	err = generic_file_open(inode, file);
	if (err)
		return err;

	if (lock_inode)
		mutex_lock(&inode->i_mutex);

	err = fuse_do_open(fc, get_node_id(inode), file, isdir);

	if (!err)
		fuse_finish_open(inode, file);

	if (lock_inode)
		mutex_unlock(&inode->i_mutex);

	return err;
}
