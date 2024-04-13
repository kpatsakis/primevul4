static inline void bnep_set_default_proto_filter(struct bnep_session *s)
{
	/* (IPv4, ARP)  */
	s->proto_filter[0].start = ETH_P_IP;
	s->proto_filter[0].end   = ETH_P_ARP;
	/* (RARP, AppleTalk) */
	s->proto_filter[1].start = ETH_P_RARP;
	s->proto_filter[1].end   = ETH_P_AARP;
	/* (IPX, IPv6) */
	s->proto_filter[2].start = ETH_P_IPX;
	s->proto_filter[2].end   = ETH_P_IPV6;
}
