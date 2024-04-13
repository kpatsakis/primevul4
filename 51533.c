void __audit_file(const struct file *file)
{
	__audit_inode(NULL, file->f_path.dentry, 0);
}
