static int icmpv6_getfrag(void *from, char *to, int offset, int len, int odd, struct sk_buff *skb)
{
	struct icmpv6_msg *msg = (struct icmpv6_msg *) from;
	struct sk_buff *org_skb = msg->skb;
	__wsum csum = 0;

	csum = skb_copy_and_csum_bits(org_skb, msg->offset + offset,
				      to, len, csum);
	skb->csum = csum_block_add(skb->csum, csum, odd);
	if (!(msg->type & ICMPV6_INFOMSG_MASK))
		nf_ct_attach(skb, org_skb);
	return 0;
}
