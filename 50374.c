static int proc_sys_revalidate(struct dentry *dentry, unsigned int flags)
{
	if (flags & LOOKUP_RCU)
		return -ECHILD;
	return !PROC_I(d_inode(dentry))->sysctl->unregistering;
}
