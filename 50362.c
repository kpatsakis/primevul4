static int proc_sys_compare(const struct dentry *dentry,
		unsigned int len, const char *str, const struct qstr *name)
{
	struct ctl_table_header *head;
	struct inode *inode;

	/* Although proc doesn't have negative dentries, rcu-walk means
	 * that inode here can be NULL */
	/* AV: can it, indeed? */
	inode = d_inode_rcu(dentry);
	if (!inode)
		return 1;
	if (name->len != len)
		return 1;
	if (memcmp(name->name, str, len))
		return 1;
	head = rcu_dereference(PROC_I(inode)->sysctl);
	return !head || !sysctl_is_seen(head);
}
