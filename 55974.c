static void build_skb_flow_key(struct flowi4 *fl4, const struct sk_buff *skb,
			       const struct sock *sk)
{
	const struct net *net = dev_net(skb->dev);
	const struct iphdr *iph = ip_hdr(skb);
	int oif = skb->dev->ifindex;
	u8 tos = RT_TOS(iph->tos);
	u8 prot = iph->protocol;
	u32 mark = skb->mark;

	__build_flow_key(net, fl4, sk, iph, oif, tos, prot, mark, 0);
}
