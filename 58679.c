struct dentry *debugfs_create_file_size(const char *name, umode_t mode,
					struct dentry *parent, void *data,
					const struct file_operations *fops,
					loff_t file_size)
{
	struct dentry *de = debugfs_create_file(name, mode, parent, data, fops);

	if (de)
		d_inode(de)->i_size = file_size;
	return de;
}
