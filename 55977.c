static void ip_do_redirect(struct dst_entry *dst, struct sock *sk, struct sk_buff *skb)
{
	struct rtable *rt;
	struct flowi4 fl4;
	const struct iphdr *iph = (const struct iphdr *) skb->data;
	struct net *net = dev_net(skb->dev);
	int oif = skb->dev->ifindex;
	u8 tos = RT_TOS(iph->tos);
	u8 prot = iph->protocol;
	u32 mark = skb->mark;

	rt = (struct rtable *) dst;

	__build_flow_key(net, &fl4, sk, iph, oif, tos, prot, mark, 0);
	__ip_do_redirect(rt, skb, &fl4, true);
}
