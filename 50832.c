static struct dentry *proc_attr_dir_lookup(struct inode *dir,
				struct dentry *dentry, unsigned int flags)
{
	return proc_pident_lookup(dir, dentry,
				  attr_dir_stuff, ARRAY_SIZE(attr_dir_stuff));
}
