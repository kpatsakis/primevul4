static inline void nf_bridge_pull_encap_header(struct sk_buff *skb)
{
	unsigned int len = nf_bridge_encap_header_len(skb);

	skb_pull(skb, len);
	skb->network_header += len;
}
