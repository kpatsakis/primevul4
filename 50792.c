static int nfnetlink_bind(struct net *net, int group)
{
	const struct nfnetlink_subsystem *ss;
	int type;

	if (group <= NFNLGRP_NONE || group > NFNLGRP_MAX)
		return 0;

	type = nfnl_group2type[group];

	rcu_read_lock();
	ss = nfnetlink_get_subsys(type << 8);
	rcu_read_unlock();
	if (!ss)
		request_module("nfnetlink-subsys-%d", type);
	return 0;
}
