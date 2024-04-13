static loff_t fuse_file_llseek(struct file *file, loff_t offset, int whence)
{
	loff_t retval;
	struct inode *inode = file_inode(file);

	/* No i_mutex protection necessary for SEEK_CUR and SEEK_SET */
	if (whence == SEEK_CUR || whence == SEEK_SET)
		return generic_file_llseek(file, offset, whence);

	mutex_lock(&inode->i_mutex);
	retval = fuse_update_attributes(inode, NULL, file, NULL);
	if (!retval)
		retval = generic_file_llseek(file, offset, whence);
	mutex_unlock(&inode->i_mutex);

	return retval;
}
