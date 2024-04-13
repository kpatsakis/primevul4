static int ip_vs_genl_set_config(struct net *net, struct nlattr **attrs)
{
	struct ip_vs_timeout_user t;

	__ip_vs_get_timeouts(net, &t);

	if (attrs[IPVS_CMD_ATTR_TIMEOUT_TCP])
		t.tcp_timeout = nla_get_u32(attrs[IPVS_CMD_ATTR_TIMEOUT_TCP]);

	if (attrs[IPVS_CMD_ATTR_TIMEOUT_TCP_FIN])
		t.tcp_fin_timeout =
			nla_get_u32(attrs[IPVS_CMD_ATTR_TIMEOUT_TCP_FIN]);

	if (attrs[IPVS_CMD_ATTR_TIMEOUT_UDP])
		t.udp_timeout = nla_get_u32(attrs[IPVS_CMD_ATTR_TIMEOUT_UDP]);

	return ip_vs_set_timeout(net, &t);
}
