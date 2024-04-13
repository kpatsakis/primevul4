void retire_sysctl_set(struct ctl_table_set *set)
{
	WARN_ON(!RB_EMPTY_ROOT(&set->dir.root));
}
