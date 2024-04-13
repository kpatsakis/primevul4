static bool is_empty_dir(struct ctl_table_header *head)
{
	return head->ctl_table[0].child == sysctl_mount_point;
}
