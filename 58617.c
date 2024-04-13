void d_prune_aliases(struct inode *inode)
{
	struct dentry *dentry;
restart:
	spin_lock(&inode->i_lock);
	hlist_for_each_entry(dentry, &inode->i_dentry, d_u.d_alias) {
		spin_lock(&dentry->d_lock);
		if (!dentry->d_lockref.count) {
			struct dentry *parent = lock_parent(dentry);
			if (likely(!dentry->d_lockref.count)) {
				__dentry_kill(dentry);
				dput(parent);
				goto restart;
			}
			if (parent)
				spin_unlock(&parent->d_lock);
		}
		spin_unlock(&dentry->d_lock);
	}
	spin_unlock(&inode->i_lock);
}
