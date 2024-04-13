static struct sk_buff *ipxitf_adjust_skbuff(struct ipx_interface *intrfc,
					    struct sk_buff *skb)
{
	struct sk_buff *skb2;
	int in_offset = (unsigned char *)ipx_hdr(skb) - skb->head;
	int out_offset = intrfc->if_ipx_offset;
	int len;

	/* Hopefully, most cases */
	if (in_offset >= out_offset)
		return skb;

	/* Need new SKB */
	len  = skb->len + out_offset;
	skb2 = alloc_skb(len, GFP_ATOMIC);
	if (skb2) {
		skb_reserve(skb2, out_offset);
		skb_reset_network_header(skb2);
		skb_reset_transport_header(skb2);
		skb_put(skb2, skb->len);
		memcpy(ipx_hdr(skb2), ipx_hdr(skb), skb->len);
		memcpy(skb2->cb, skb->cb, sizeof(skb->cb));
	}
	kfree_skb(skb);
	return skb2;
}
