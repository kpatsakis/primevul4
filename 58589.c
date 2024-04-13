struct dentry *d_alloc_name(struct dentry *parent, const char *name)
{
	struct qstr q;

	q.name = name;
	q.hash_len = hashlen_string(parent, name);
	return d_alloc(parent, &q);
}
