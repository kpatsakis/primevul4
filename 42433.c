static int ip_vs_set_timeout(struct net *net, struct ip_vs_timeout_user *u)
{
#if defined(CONFIG_IP_VS_PROTO_TCP) || defined(CONFIG_IP_VS_PROTO_UDP)
	struct ip_vs_proto_data *pd;
#endif

	IP_VS_DBG(2, "Setting timeout tcp:%d tcpfin:%d udp:%d\n",
		  u->tcp_timeout,
		  u->tcp_fin_timeout,
		  u->udp_timeout);

#ifdef CONFIG_IP_VS_PROTO_TCP
	if (u->tcp_timeout) {
		pd = ip_vs_proto_data_get(net, IPPROTO_TCP);
		pd->timeout_table[IP_VS_TCP_S_ESTABLISHED]
			= u->tcp_timeout * HZ;
	}

	if (u->tcp_fin_timeout) {
		pd = ip_vs_proto_data_get(net, IPPROTO_TCP);
		pd->timeout_table[IP_VS_TCP_S_FIN_WAIT]
			= u->tcp_fin_timeout * HZ;
	}
#endif

#ifdef CONFIG_IP_VS_PROTO_UDP
	if (u->udp_timeout) {
		pd = ip_vs_proto_data_get(net, IPPROTO_UDP);
		pd->timeout_table[IP_VS_UDP_S_NORMAL]
			= u->udp_timeout * HZ;
	}
#endif
	return 0;
}
