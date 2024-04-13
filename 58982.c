set_lookup(struct ctl_table_root *root)
{
	return &current_user_ns()->set;
}
