struct dentry *lookup_one_len(const char *name, struct dentry *base, int len)
{
	int err;
	struct qstr this;

	WARN_ON_ONCE(!mutex_is_locked(&base->d_inode->i_mutex));

	err = __lookup_one_len(name, &this, base, len);
	if (err)
		return ERR_PTR(err);

	err = exec_permission(base->d_inode);
	if (err)
		return ERR_PTR(err);
	return __lookup_hash(&this, base, NULL);
}
