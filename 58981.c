static int set_is_seen(struct ctl_table_set *set)
{
	return &current_user_ns()->set == set;
}
