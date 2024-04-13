void ip_rt_send_redirect(struct sk_buff *skb)
{
	struct rtable *rt = skb_rtable(skb);
	struct in_device *in_dev;
	struct inet_peer *peer;
	struct net *net;
	int log_martians;
	int vif;

	rcu_read_lock();
	in_dev = __in_dev_get_rcu(rt->dst.dev);
	if (!in_dev || !IN_DEV_TX_REDIRECTS(in_dev)) {
		rcu_read_unlock();
		return;
	}
	log_martians = IN_DEV_LOG_MARTIANS(in_dev);
	vif = l3mdev_master_ifindex_rcu(rt->dst.dev);
	rcu_read_unlock();

	net = dev_net(rt->dst.dev);
	peer = inet_getpeer_v4(net->ipv4.peers, ip_hdr(skb)->saddr, vif, 1);
	if (!peer) {
		icmp_send(skb, ICMP_REDIRECT, ICMP_REDIR_HOST,
			  rt_nexthop(rt, ip_hdr(skb)->daddr));
		return;
	}

	/* No redirected packets during ip_rt_redirect_silence;
	 * reset the algorithm.
	 */
	if (time_after(jiffies, peer->rate_last + ip_rt_redirect_silence))
		peer->rate_tokens = 0;

	/* Too many ignored redirects; do not send anything
	 * set dst.rate_last to the last seen redirected packet.
	 */
	if (peer->rate_tokens >= ip_rt_redirect_number) {
		peer->rate_last = jiffies;
		goto out_put_peer;
	}

	/* Check for load limit; set rate_last to the latest sent
	 * redirect.
	 */
	if (peer->rate_tokens == 0 ||
	    time_after(jiffies,
		       (peer->rate_last +
			(ip_rt_redirect_load << peer->rate_tokens)))) {
		__be32 gw = rt_nexthop(rt, ip_hdr(skb)->daddr);

		icmp_send(skb, ICMP_REDIRECT, ICMP_REDIR_HOST, gw);
		peer->rate_last = jiffies;
		++peer->rate_tokens;
#ifdef CONFIG_IP_ROUTE_VERBOSE
		if (log_martians &&
		    peer->rate_tokens == ip_rt_redirect_number)
			net_warn_ratelimited("host %pI4/if%d ignores redirects for %pI4 to %pI4\n",
					     &ip_hdr(skb)->saddr, inet_iif(skb),
					     &ip_hdr(skb)->daddr, &gw);
#endif
	}
out_put_peer:
	inet_putpeer(peer);
}
