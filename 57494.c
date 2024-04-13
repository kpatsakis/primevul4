int ip6_output(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	struct net_device *dev = skb_dst(skb)->dev;
	struct inet6_dev *idev = ip6_dst_idev(skb_dst(skb));

	if (unlikely(idev->cnf.disable_ipv6)) {
		IP6_INC_STATS(net, idev, IPSTATS_MIB_OUTDISCARDS);
		kfree_skb(skb);
		return 0;
	}

	return NF_HOOK_COND(NFPROTO_IPV6, NF_INET_POST_ROUTING,
			    net, sk, skb, NULL, dev,
			    ip6_finish_output,
			    !(IP6CB(skb)->flags & IP6SKB_REROUTED));
}
