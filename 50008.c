int ip6_err_gen_icmpv6_unreach(struct sk_buff *skb, int nhs, int type,
			       unsigned int data_len)
{
	struct in6_addr temp_saddr;
	struct rt6_info *rt;
	struct sk_buff *skb2;
	u32 info = 0;

	if (!pskb_may_pull(skb, nhs + sizeof(struct ipv6hdr) + 8))
		return 1;

	/* RFC 4884 (partial) support for ICMP extensions */
	if (data_len < 128 || (data_len & 7) || skb->len < data_len)
		data_len = 0;

	skb2 = data_len ? skb_copy(skb, GFP_ATOMIC) : skb_clone(skb, GFP_ATOMIC);

	if (!skb2)
		return 1;

	skb_dst_drop(skb2);
	skb_pull(skb2, nhs);
	skb_reset_network_header(skb2);

	rt = rt6_lookup(dev_net(skb->dev), &ipv6_hdr(skb2)->saddr, NULL, 0, 0);

	if (rt && rt->dst.dev)
		skb2->dev = rt->dst.dev;

	ipv6_addr_set_v4mapped(ip_hdr(skb)->saddr, &temp_saddr);

	if (data_len) {
		/* RFC 4884 (partial) support :
		 * insert 0 padding at the end, before the extensions
		 */
		__skb_push(skb2, nhs);
		skb_reset_network_header(skb2);
		memmove(skb2->data, skb2->data + nhs, data_len - nhs);
		memset(skb2->data + data_len - nhs, 0, nhs);
		/* RFC 4884 4.5 : Length is measured in 64-bit words,
		 * and stored in reserved[0]
		 */
		info = (data_len/8) << 24;
	}
	if (type == ICMP_TIME_EXCEEDED)
		icmp6_send(skb2, ICMPV6_TIME_EXCEED, ICMPV6_EXC_HOPLIMIT,
			   info, &temp_saddr);
	else
		icmp6_send(skb2, ICMPV6_DEST_UNREACH, ICMPV6_ADDR_UNREACH,
			   info, &temp_saddr);
	if (rt)
		ip6_rt_put(rt);

	kfree_skb(skb2);

	return 0;
}
