int rawv6_mh_filter_register(mh_filter_t filter)
{
	rcu_assign_pointer(mh_filter, filter);
	return 0;
}
