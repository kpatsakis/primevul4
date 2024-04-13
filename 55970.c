static void __ipv4_sk_update_pmtu(struct sk_buff *skb, struct sock *sk, u32 mtu)
{
	const struct iphdr *iph = (const struct iphdr *) skb->data;
	struct flowi4 fl4;
	struct rtable *rt;

	__build_flow_key(sock_net(sk), &fl4, sk, iph, 0, 0, 0, 0, 0);

	if (!fl4.flowi4_mark)
		fl4.flowi4_mark = IP4_REPLY_MARK(sock_net(sk), skb->mark);

	rt = __ip_route_output_key(sock_net(sk), &fl4);
	if (!IS_ERR(rt)) {
		__ip_rt_update_pmtu(rt, &fl4, mtu);
		ip_rt_put(rt);
	}
}
