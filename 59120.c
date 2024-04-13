static inline bool ipv4_datagram_support_addr(struct sock_exterr_skb *serr)
{
	return serr->ee.ee_origin == SO_EE_ORIGIN_ICMP ||
	       serr->ee.ee_origin == SO_EE_ORIGIN_LOCAL || serr->port;
}
