static struct dentry *lookup_dcache(const struct qstr *name,
				    struct dentry *dir,
				    unsigned int flags)
{
	struct dentry *dentry;
	int error;

	dentry = d_lookup(dir, name);
	if (dentry) {
		if (dentry->d_flags & DCACHE_OP_REVALIDATE) {
			error = d_revalidate(dentry, flags);
			if (unlikely(error <= 0)) {
				if (!error)
					d_invalidate(dentry);
				dput(dentry);
				return ERR_PTR(error);
			}
		}
	}
	return dentry;
}
