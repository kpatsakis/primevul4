static loff_t block_llseek(struct file *file, loff_t offset, int whence)
{
	struct inode *bd_inode = file->f_mapping->host;
	loff_t retval;

	mutex_lock(&bd_inode->i_mutex);
	retval = fixed_size_llseek(file, offset, whence, i_size_read(bd_inode));
	mutex_unlock(&bd_inode->i_mutex);
	return retval;
}
