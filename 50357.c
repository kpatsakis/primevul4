lookup_header_set(struct ctl_table_root *root)
{
	struct ctl_table_set *set = &root->default_set;
	if (root->lookup)
		set = root->lookup(root);
	return set;
}
