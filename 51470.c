static struct dentry *lookup_slow(const struct qstr *name,
				  struct dentry *dir,
				  unsigned int flags)
{
	struct dentry *dentry;
	inode_lock(dir->d_inode);
	dentry = d_lookup(dir, name);
	if (unlikely(dentry)) {
		if ((dentry->d_flags & DCACHE_OP_REVALIDATE) &&
		    !(flags & LOOKUP_NO_REVAL)) {
			int error = d_revalidate(dentry, flags);
			if (unlikely(error <= 0)) {
				if (!error)
					d_invalidate(dentry);
				dput(dentry);
				dentry = ERR_PTR(error);
			}
		}
		if (dentry) {
			inode_unlock(dir->d_inode);
			return dentry;
		}
	}
	dentry = d_alloc(dir, name);
	if (unlikely(!dentry)) {
		inode_unlock(dir->d_inode);
		return ERR_PTR(-ENOMEM);
	}
	dentry = lookup_real(dir->d_inode, dentry, flags);
	inode_unlock(dir->d_inode);
	return dentry;
}
