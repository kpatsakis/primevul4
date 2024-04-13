static enum d_walk_ret umount_check(void *_data, struct dentry *dentry)
{
	/* it has busy descendents; complain about those instead */
	if (!list_empty(&dentry->d_subdirs))
		return D_WALK_CONTINUE;

	/* root with refcount 1 is fine */
	if (dentry == _data && dentry->d_lockref.count == 1)
		return D_WALK_CONTINUE;

	printk(KERN_ERR "BUG: Dentry %p{i=%lx,n=%pd} "
			" still in use (%d) [unmount of %s %s]\n",
		       dentry,
		       dentry->d_inode ?
		       dentry->d_inode->i_ino : 0UL,
		       dentry,
		       dentry->d_lockref.count,
		       dentry->d_sb->s_type->name,
		       dentry->d_sb->s_id);
	WARN_ON(1);
	return D_WALK_CONTINUE;
}
