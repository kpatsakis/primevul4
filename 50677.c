static bool packet_extra_vlan_len_allowed(const struct net_device *dev,
					  struct sk_buff *skb)
{
	/* Earlier code assumed this would be a VLAN pkt, double-check
	 * this now that we have the actual packet in hand. We can only
	 * do this check on Ethernet devices.
	 */
	if (unlikely(dev->type != ARPHRD_ETHER))
		return false;

	skb_reset_mac_header(skb);
	return likely(eth_hdr(skb)->h_proto == htons(ETH_P_8021Q));
}
