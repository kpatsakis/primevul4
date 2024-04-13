int rawv6_mh_filter_unregister(mh_filter_t filter)
{
	RCU_INIT_POINTER(mh_filter, NULL);
	synchronize_rcu();
	return 0;
}
