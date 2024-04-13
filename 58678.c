struct dentry *debugfs_create_file(const char *name, umode_t mode,
				   struct dentry *parent, void *data,
				   const struct file_operations *fops)
{

	return __debugfs_create_file(name, mode, parent, data,
				fops ? &debugfs_full_proxy_file_operations :
					&debugfs_noop_file_operations,
				fops);
}
