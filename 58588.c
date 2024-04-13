struct dentry *d_alloc_cursor(struct dentry * parent)
{
	struct dentry *dentry = __d_alloc(parent->d_sb, NULL);
	if (dentry) {
		dentry->d_flags |= DCACHE_RCUACCESS | DCACHE_DENTRY_CURSOR;
		dentry->d_parent = dget(parent);
	}
	return dentry;
}
