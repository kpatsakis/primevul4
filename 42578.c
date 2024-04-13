static inline void nf_bridge_save_header(struct sk_buff *skb)
{
	int header_size = ETH_HLEN + nf_bridge_encap_header_len(skb);

	skb_copy_from_linear_data_offset(skb, -header_size,
					 skb->nf_bridge->data, header_size);
}
