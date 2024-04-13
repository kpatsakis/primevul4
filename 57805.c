static void sctp_v6_from_skb(union sctp_addr *addr, struct sk_buff *skb,
			     int is_saddr)
{
	/* Always called on head skb, so this is safe */
	struct sctphdr *sh = sctp_hdr(skb);
	struct sockaddr_in6 *sa = &addr->v6;

	addr->v6.sin6_family = AF_INET6;
	addr->v6.sin6_flowinfo = 0; /* FIXME */
	addr->v6.sin6_scope_id = ((struct inet6_skb_parm *)skb->cb)->iif;

	if (is_saddr) {
		sa->sin6_port = sh->source;
		sa->sin6_addr = ipv6_hdr(skb)->saddr;
	} else {
		sa->sin6_port = sh->dest;
		sa->sin6_addr = ipv6_hdr(skb)->daddr;
	}
}
