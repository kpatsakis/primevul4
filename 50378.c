struct ctl_table_header *register_sysctl(const char *path, struct ctl_table *table)
{
	return __register_sysctl_table(&sysctl_table_root.default_set,
					path, table);
}
