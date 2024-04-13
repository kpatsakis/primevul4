static struct sock *ping_lookup(struct net *net, struct sk_buff *skb, u16 ident)
{
	struct hlist_nulls_head *hslot = ping_hashslot(&ping_table, net, ident);
	struct sock *sk = NULL;
	struct inet_sock *isk;
	struct hlist_nulls_node *hnode;
	int dif = skb->dev->ifindex;

	if (skb->protocol == htons(ETH_P_IP)) {
		pr_debug("try to find: num = %d, daddr = %pI4, dif = %d\n",
			 (int)ident, &ip_hdr(skb)->daddr, dif);
#if IS_ENABLED(CONFIG_IPV6)
	} else if (skb->protocol == htons(ETH_P_IPV6)) {
		pr_debug("try to find: num = %d, daddr = %pI6c, dif = %d\n",
			 (int)ident, &ipv6_hdr(skb)->daddr, dif);
#endif
	}

	read_lock_bh(&ping_table.lock);

	ping_portaddr_for_each_entry(sk, hnode, hslot) {
		isk = inet_sk(sk);

		pr_debug("iterate\n");
		if (isk->inet_num != ident)
			continue;

		if (skb->protocol == htons(ETH_P_IP) &&
		    sk->sk_family == AF_INET) {
			pr_debug("found: %p: num=%d, daddr=%pI4, dif=%d\n", sk,
				 (int) isk->inet_num, &isk->inet_rcv_saddr,
				 sk->sk_bound_dev_if);

			if (isk->inet_rcv_saddr &&
			    isk->inet_rcv_saddr != ip_hdr(skb)->daddr)
				continue;
#if IS_ENABLED(CONFIG_IPV6)
		} else if (skb->protocol == htons(ETH_P_IPV6) &&
			   sk->sk_family == AF_INET6) {

			pr_debug("found: %p: num=%d, daddr=%pI6c, dif=%d\n", sk,
				 (int) isk->inet_num,
				 &sk->sk_v6_rcv_saddr,
				 sk->sk_bound_dev_if);

			if (!ipv6_addr_any(&sk->sk_v6_rcv_saddr) &&
			    !ipv6_addr_equal(&sk->sk_v6_rcv_saddr,
					     &ipv6_hdr(skb)->daddr))
				continue;
#endif
		} else {
			continue;
		}

		if (sk->sk_bound_dev_if && sk->sk_bound_dev_if != dif)
			continue;

		sock_hold(sk);
		goto exit;
	}

	sk = NULL;
exit:
	read_unlock_bh(&ping_table.lock);

	return sk;
}
