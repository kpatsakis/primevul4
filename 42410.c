static int ip_vs_genl_del_daemon(struct net *net, struct nlattr **attrs)
{
	if (!attrs[IPVS_DAEMON_ATTR_STATE])
		return -EINVAL;

	return stop_sync_thread(net,
				nla_get_u32(attrs[IPVS_DAEMON_ATTR_STATE]));
}
