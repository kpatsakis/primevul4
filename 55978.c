static int ip_error(struct sk_buff *skb)
{
	struct in_device *in_dev = __in_dev_get_rcu(skb->dev);
	struct rtable *rt = skb_rtable(skb);
	struct inet_peer *peer;
	unsigned long now;
	struct net *net;
	bool send;
	int code;

	/* IP on this device is disabled. */
	if (!in_dev)
		goto out;

	net = dev_net(rt->dst.dev);
	if (!IN_DEV_FORWARD(in_dev)) {
		switch (rt->dst.error) {
		case EHOSTUNREACH:
			__IP_INC_STATS(net, IPSTATS_MIB_INADDRERRORS);
			break;

		case ENETUNREACH:
			__IP_INC_STATS(net, IPSTATS_MIB_INNOROUTES);
			break;
		}
		goto out;
	}

	switch (rt->dst.error) {
	case EINVAL:
	default:
		goto out;
	case EHOSTUNREACH:
		code = ICMP_HOST_UNREACH;
		break;
	case ENETUNREACH:
		code = ICMP_NET_UNREACH;
		__IP_INC_STATS(net, IPSTATS_MIB_INNOROUTES);
		break;
	case EACCES:
		code = ICMP_PKT_FILTERED;
		break;
	}

	peer = inet_getpeer_v4(net->ipv4.peers, ip_hdr(skb)->saddr,
			       l3mdev_master_ifindex(skb->dev), 1);

	send = true;
	if (peer) {
		now = jiffies;
		peer->rate_tokens += now - peer->rate_last;
		if (peer->rate_tokens > ip_rt_error_burst)
			peer->rate_tokens = ip_rt_error_burst;
		peer->rate_last = now;
		if (peer->rate_tokens >= ip_rt_error_cost)
			peer->rate_tokens -= ip_rt_error_cost;
		else
			send = false;
		inet_putpeer(peer);
	}
	if (send)
		icmp_send(skb, ICMP_DEST_UNREACH, code, 0);

out:	kfree_skb(skb);
	return 0;
}
