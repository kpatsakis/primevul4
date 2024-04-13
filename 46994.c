static void sctp_v4_dst_saddr(union sctp_addr *saddr, struct flowi4 *fl4,
			      __be16 port)
{
	saddr->v4.sin_family = AF_INET;
	saddr->v4.sin_port = port;
	saddr->v4.sin_addr.s_addr = fl4->saddr;
}
