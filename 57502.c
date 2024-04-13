int ip6_xmit(const struct sock *sk, struct sk_buff *skb, struct flowi6 *fl6,
	     __u32 mark, struct ipv6_txoptions *opt, int tclass)
{
	struct net *net = sock_net(sk);
	const struct ipv6_pinfo *np = inet6_sk(sk);
	struct in6_addr *first_hop = &fl6->daddr;
	struct dst_entry *dst = skb_dst(skb);
	struct ipv6hdr *hdr;
	u8  proto = fl6->flowi6_proto;
	int seg_len = skb->len;
	int hlimit = -1;
	u32 mtu;

	if (opt) {
		unsigned int head_room;

		/* First: exthdrs may take lots of space (~8K for now)
		   MAX_HEADER is not enough.
		 */
		head_room = opt->opt_nflen + opt->opt_flen;
		seg_len += head_room;
		head_room += sizeof(struct ipv6hdr) + LL_RESERVED_SPACE(dst->dev);

		if (skb_headroom(skb) < head_room) {
			struct sk_buff *skb2 = skb_realloc_headroom(skb, head_room);
			if (!skb2) {
				IP6_INC_STATS(net, ip6_dst_idev(skb_dst(skb)),
					      IPSTATS_MIB_OUTDISCARDS);
				kfree_skb(skb);
				return -ENOBUFS;
			}
			consume_skb(skb);
			skb = skb2;
			/* skb_set_owner_w() changes sk->sk_wmem_alloc atomically,
			 * it is safe to call in our context (socket lock not held)
			 */
			skb_set_owner_w(skb, (struct sock *)sk);
		}
		if (opt->opt_flen)
			ipv6_push_frag_opts(skb, opt, &proto);
		if (opt->opt_nflen)
			ipv6_push_nfrag_opts(skb, opt, &proto, &first_hop,
					     &fl6->saddr);
	}

	skb_push(skb, sizeof(struct ipv6hdr));
	skb_reset_network_header(skb);
	hdr = ipv6_hdr(skb);

	/*
	 *	Fill in the IPv6 header
	 */
	if (np)
		hlimit = np->hop_limit;
	if (hlimit < 0)
		hlimit = ip6_dst_hoplimit(dst);

	ip6_flow_hdr(hdr, tclass, ip6_make_flowlabel(net, skb, fl6->flowlabel,
						     np->autoflowlabel, fl6));

	hdr->payload_len = htons(seg_len);
	hdr->nexthdr = proto;
	hdr->hop_limit = hlimit;

	hdr->saddr = fl6->saddr;
	hdr->daddr = *first_hop;

	skb->protocol = htons(ETH_P_IPV6);
	skb->priority = sk->sk_priority;
	skb->mark = mark;

	mtu = dst_mtu(dst);
	if ((skb->len <= mtu) || skb->ignore_df || skb_is_gso(skb)) {
		IP6_UPD_PO_STATS(net, ip6_dst_idev(skb_dst(skb)),
			      IPSTATS_MIB_OUT, skb->len);

		/* if egress device is enslaved to an L3 master device pass the
		 * skb to its handler for processing
		 */
		skb = l3mdev_ip6_out((struct sock *)sk, skb);
		if (unlikely(!skb))
			return 0;

		/* hooks should never assume socket lock is held.
		 * we promote our socket to non const
		 */
		return NF_HOOK(NFPROTO_IPV6, NF_INET_LOCAL_OUT,
			       net, (struct sock *)sk, skb, NULL, dst->dev,
			       dst_output);
	}

	skb->dev = dst->dev;
	/* ipv6_local_error() does not require socket lock,
	 * we promote our socket to non const
	 */
	ipv6_local_error((struct sock *)sk, EMSGSIZE, fl6, mtu);

	IP6_INC_STATS(net, ip6_dst_idev(skb_dst(skb)), IPSTATS_MIB_FRAGFAILS);
	kfree_skb(skb);
	return -EMSGSIZE;
}
