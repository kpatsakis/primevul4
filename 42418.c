static int ip_vs_genl_new_daemon(struct net *net, struct nlattr **attrs)
{
	if (!(attrs[IPVS_DAEMON_ATTR_STATE] &&
	      attrs[IPVS_DAEMON_ATTR_MCAST_IFN] &&
	      attrs[IPVS_DAEMON_ATTR_SYNC_ID]))
		return -EINVAL;

	return start_sync_thread(net,
				 nla_get_u32(attrs[IPVS_DAEMON_ATTR_STATE]),
				 nla_data(attrs[IPVS_DAEMON_ATTR_MCAST_IFN]),
				 nla_get_u32(attrs[IPVS_DAEMON_ATTR_SYNC_ID]));
}
