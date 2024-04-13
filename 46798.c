void __udp6_lib_err(struct sk_buff *skb, struct inet6_skb_parm *opt,
		    u8 type, u8 code, int offset, __be32 info,
		    struct udp_table *udptable)
{
	struct ipv6_pinfo *np;
	const struct ipv6hdr *hdr = (const struct ipv6hdr *)skb->data;
	const struct in6_addr *saddr = &hdr->saddr;
	const struct in6_addr *daddr = &hdr->daddr;
	struct udphdr *uh = (struct udphdr *)(skb->data+offset);
	struct sock *sk;
	int err;
	struct net *net = dev_net(skb->dev);

	sk = __udp6_lib_lookup(net, daddr, uh->dest,
			       saddr, uh->source, inet6_iif(skb), udptable);
	if (!sk) {
		ICMP6_INC_STATS_BH(net, __in6_dev_get(skb->dev),
				   ICMP6_MIB_INERRORS);
		return;
	}

	if (type == ICMPV6_PKT_TOOBIG) {
		if (!ip6_sk_accept_pmtu(sk))
			goto out;
		ip6_sk_update_pmtu(skb, sk, info);
	}
	if (type == NDISC_REDIRECT) {
		ip6_sk_redirect(skb, sk);
		goto out;
	}

	np = inet6_sk(sk);

	if (!icmpv6_err_convert(type, code, &err) && !np->recverr)
		goto out;

	if (sk->sk_state != TCP_ESTABLISHED && !np->recverr)
		goto out;

	if (np->recverr)
		ipv6_icmp_error(sk, skb, err, uh->dest, ntohl(info), (u8 *)(uh+1));

	sk->sk_err = err;
	sk->sk_error_report(sk);
out:
	sock_put(sk);
}
