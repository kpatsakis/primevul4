static inline __be16 vlan_proto(const struct sk_buff *skb)
{
	if (vlan_tx_tag_present(skb))
		return skb->protocol;
	else if (skb->protocol == htons(ETH_P_8021Q))
		return vlan_eth_hdr(skb)->h_vlan_encapsulated_proto;
	else
		return 0;
}
