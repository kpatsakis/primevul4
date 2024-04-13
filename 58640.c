static void do_one_tree(struct dentry *dentry)
{
	shrink_dcache_parent(dentry);
	d_walk(dentry, dentry, umount_check, NULL);
	d_drop(dentry);
	dput(dentry);
}
