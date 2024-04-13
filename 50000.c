void icmpv6_param_prob(struct sk_buff *skb, u8 code, int pos)
{
	icmp6_send(skb, ICMPV6_PARAMPROB, code, pos, NULL);
	kfree_skb(skb);
}
