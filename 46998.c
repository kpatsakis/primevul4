static void sctp_v4_from_skb(union sctp_addr *addr, struct sk_buff *skb,
			     int is_saddr)
{
	void *from;
	__be16 *port;
	struct sctphdr *sh;

	port = &addr->v4.sin_port;
	addr->v4.sin_family = AF_INET;

	sh = sctp_hdr(skb);
	if (is_saddr) {
		*port  = sh->source;
		from = &ip_hdr(skb)->saddr;
	} else {
		*port = sh->dest;
		from = &ip_hdr(skb)->daddr;
	}
	memcpy(&addr->v4.sin_addr.s_addr, from, sizeof(struct in_addr));
}
