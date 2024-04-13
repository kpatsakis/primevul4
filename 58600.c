void d_genocide(struct dentry *parent)
{
	d_walk(parent, parent, d_genocide_kill, NULL);
}
