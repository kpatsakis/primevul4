isofs_hash_ms(const struct dentry *dentry, struct qstr *qstr)
{
	return isofs_hash_common(qstr, 1);
}
