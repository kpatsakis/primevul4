static void sctp_inet6_skb_msgname(struct sk_buff *skb, char *msgname,
				   int *addr_len)
{
	union sctp_addr *addr;
	struct sctphdr *sh;

	if (!msgname)
		return;

	addr = (union sctp_addr *)msgname;
	sh = sctp_hdr(skb);

	if (ip_hdr(skb)->version == 4) {
		addr->v4.sin_family = AF_INET;
		addr->v4.sin_port = sh->source;
		addr->v4.sin_addr.s_addr = ip_hdr(skb)->saddr;
	} else {
		addr->v6.sin6_family = AF_INET6;
		addr->v6.sin6_flowinfo = 0;
		addr->v6.sin6_port = sh->source;
		addr->v6.sin6_addr = ipv6_hdr(skb)->saddr;
		if (ipv6_addr_type(&addr->v6.sin6_addr) & IPV6_ADDR_LINKLOCAL) {
			addr->v6.sin6_scope_id = sctp_v6_skb_iif(skb);
		}
	}

	*addr_len = sctp_v6_addr_to_user(sctp_sk(skb->sk), addr);
}
