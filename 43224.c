isofs_hashi_ms(const struct dentry *dentry, struct qstr *qstr)
{
	return isofs_hashi_common(qstr, 1);
}
