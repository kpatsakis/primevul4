void setup_sysctl_set(struct ctl_table_set *set,
	struct ctl_table_root *root,
	int (*is_seen)(struct ctl_table_set *))
{
	memset(set, 0, sizeof(*set));
	set->is_seen = is_seen;
	init_header(&set->dir.header, root, set, NULL, root_table);
}
