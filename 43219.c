isofs_hash(const struct dentry *dentry, struct qstr *qstr)
{
	return isofs_hash_common(qstr, 0);
}
