struct dentry *d_exact_alias(struct dentry *entry, struct inode *inode)
{
	struct dentry *alias;
	unsigned int hash = entry->d_name.hash;

	spin_lock(&inode->i_lock);
	hlist_for_each_entry(alias, &inode->i_dentry, d_u.d_alias) {
		/*
		 * Don't need alias->d_lock here, because aliases with
		 * d_parent == entry->d_parent are not subject to name or
		 * parent changes, because the parent inode i_mutex is held.
		 */
		if (alias->d_name.hash != hash)
			continue;
		if (alias->d_parent != entry->d_parent)
			continue;
		if (!d_same_name(alias, entry->d_parent, &entry->d_name))
			continue;
		spin_lock(&alias->d_lock);
		if (!d_unhashed(alias)) {
			spin_unlock(&alias->d_lock);
			alias = NULL;
		} else {
			__dget_dlock(alias);
			__d_rehash(alias);
			spin_unlock(&alias->d_lock);
		}
		spin_unlock(&inode->i_lock);
		return alias;
	}
	spin_unlock(&inode->i_lock);
	return NULL;
}
