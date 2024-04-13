static int proc_sys_delete(const struct dentry *dentry)
{
	return !!PROC_I(d_inode(dentry))->sysctl->unregistering;
}
