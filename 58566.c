static struct dentry *__d_find_alias(struct inode *inode)
{
	struct dentry *alias, *discon_alias;

again:
	discon_alias = NULL;
	hlist_for_each_entry(alias, &inode->i_dentry, d_u.d_alias) {
		spin_lock(&alias->d_lock);
 		if (S_ISDIR(inode->i_mode) || !d_unhashed(alias)) {
			if (IS_ROOT(alias) &&
			    (alias->d_flags & DCACHE_DISCONNECTED)) {
				discon_alias = alias;
			} else {
				__dget_dlock(alias);
				spin_unlock(&alias->d_lock);
				return alias;
			}
		}
		spin_unlock(&alias->d_lock);
	}
	if (discon_alias) {
		alias = discon_alias;
		spin_lock(&alias->d_lock);
		if (S_ISDIR(inode->i_mode) || !d_unhashed(alias)) {
			__dget_dlock(alias);
			spin_unlock(&alias->d_lock);
			return alias;
		}
		spin_unlock(&alias->d_lock);
		goto again;
	}
	return NULL;
}
