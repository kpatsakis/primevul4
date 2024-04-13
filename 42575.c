int nf_bridge_copy_header(struct sk_buff *skb)
{
	int err;
	unsigned int header_size;

	nf_bridge_update_protocol(skb);
	header_size = ETH_HLEN + nf_bridge_encap_header_len(skb);
	err = skb_cow_head(skb, header_size);
	if (err)
		return err;

	skb_copy_to_linear_data_offset(skb, -header_size,
				       skb->nf_bridge->data, header_size);
	__skb_push(skb, nf_bridge_encap_header_len(skb));
	return 0;
}
