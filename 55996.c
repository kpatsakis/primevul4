void ipv4_sk_update_pmtu(struct sk_buff *skb, struct sock *sk, u32 mtu)
{
	const struct iphdr *iph = (const struct iphdr *) skb->data;
	struct flowi4 fl4;
	struct rtable *rt;
	struct dst_entry *odst = NULL;
	bool new = false;
	struct net *net = sock_net(sk);

	bh_lock_sock(sk);

	if (!ip_sk_accept_pmtu(sk))
		goto out;

	odst = sk_dst_get(sk);

	if (sock_owned_by_user(sk) || !odst) {
		__ipv4_sk_update_pmtu(skb, sk, mtu);
		goto out;
	}

	__build_flow_key(net, &fl4, sk, iph, 0, 0, 0, 0, 0);

	rt = (struct rtable *)odst;
	if (odst->obsolete && !odst->ops->check(odst, 0)) {
		rt = ip_route_output_flow(sock_net(sk), &fl4, sk);
		if (IS_ERR(rt))
			goto out;

		new = true;
	}

	__ip_rt_update_pmtu((struct rtable *) rt->dst.path, &fl4, mtu);

	if (!dst_check(&rt->dst, 0)) {
		if (new)
			dst_release(&rt->dst);

		rt = ip_route_output_flow(sock_net(sk), &fl4, sk);
		if (IS_ERR(rt))
			goto out;

		new = true;
	}

	if (new)
		sk_dst_set(sk, &rt->dst);

out:
	bh_unlock_sock(sk);
	dst_release(odst);
}
