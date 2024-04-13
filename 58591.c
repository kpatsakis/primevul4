struct dentry *d_alloc_pseudo(struct super_block *sb, const struct qstr *name)
{
	return __d_alloc(sb, name);
}
