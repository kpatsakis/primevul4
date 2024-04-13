isofs_hashi(const struct dentry *dentry, struct qstr *qstr)
{
	return isofs_hashi_common(qstr, 0);
}
