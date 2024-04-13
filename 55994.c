void ipv4_redirect(struct sk_buff *skb, struct net *net,
		   int oif, u32 mark, u8 protocol, int flow_flags)
{
	const struct iphdr *iph = (const struct iphdr *) skb->data;
	struct flowi4 fl4;
	struct rtable *rt;

	__build_flow_key(net, &fl4, NULL, iph, oif,
			 RT_TOS(iph->tos), protocol, mark, flow_flags);
	rt = __ip_route_output_key(net, &fl4);
	if (!IS_ERR(rt)) {
		__ip_do_redirect(rt, skb, &fl4, false);
		ip_rt_put(rt);
	}
}
