__ip_vs_get_timeouts(struct net *net, struct ip_vs_timeout_user *u)
{
#if defined(CONFIG_IP_VS_PROTO_TCP) || defined(CONFIG_IP_VS_PROTO_UDP)
	struct ip_vs_proto_data *pd;
#endif

#ifdef CONFIG_IP_VS_PROTO_TCP
	pd = ip_vs_proto_data_get(net, IPPROTO_TCP);
	u->tcp_timeout = pd->timeout_table[IP_VS_TCP_S_ESTABLISHED] / HZ;
	u->tcp_fin_timeout = pd->timeout_table[IP_VS_TCP_S_FIN_WAIT] / HZ;
#endif
#ifdef CONFIG_IP_VS_PROTO_UDP
	pd = ip_vs_proto_data_get(net, IPPROTO_UDP);
	u->udp_timeout =
			pd->timeout_table[IP_VS_UDP_S_NORMAL] / HZ;
#endif
}
