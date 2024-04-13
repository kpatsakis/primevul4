static struct dentry *pipefs_mount(struct file_system_type *fs_type,
			 int flags, const char *dev_name, void *data)
{
	return mount_pseudo(fs_type, "pipe:", &pipefs_ops,
			&pipefs_dentry_operations, PIPEFS_MAGIC);
}
