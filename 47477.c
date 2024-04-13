static void ndisc_send_skb(struct sk_buff *skb,
			   const struct in6_addr *daddr,
			   const struct in6_addr *saddr)
{
	struct dst_entry *dst = skb_dst(skb);
	struct net *net = dev_net(skb->dev);
	struct sock *sk = net->ipv6.ndisc_sk;
	struct inet6_dev *idev;
	int err;
	struct icmp6hdr *icmp6h = icmp6_hdr(skb);
	u8 type;

	type = icmp6h->icmp6_type;

	if (!dst) {
		struct flowi6 fl6;

		icmpv6_flow_init(sk, &fl6, type, saddr, daddr, skb->dev->ifindex);
		dst = icmp6_dst_alloc(skb->dev, &fl6);
		if (IS_ERR(dst)) {
			kfree_skb(skb);
			return;
		}

		skb_dst_set(skb, dst);
	}

	icmp6h->icmp6_cksum = csum_ipv6_magic(saddr, daddr, skb->len,
					      IPPROTO_ICMPV6,
					      csum_partial(icmp6h,
							   skb->len, 0));

	ip6_nd_hdr(skb, saddr, daddr, inet6_sk(sk)->hop_limit, skb->len);

	rcu_read_lock();
	idev = __in6_dev_get(dst->dev);
	IP6_UPD_PO_STATS(net, idev, IPSTATS_MIB_OUT, skb->len);

	err = NF_HOOK(NFPROTO_IPV6, NF_INET_LOCAL_OUT, skb, NULL, dst->dev,
		      dst_output);
	if (!err) {
		ICMP6MSGOUT_INC_STATS(net, idev, type);
		ICMP6_INC_STATS(net, idev, ICMP6_MIB_OUTMSGS);
	}

	rcu_read_unlock();
}
