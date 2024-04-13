static inline void skb_bond_set_mac_by_master(struct sk_buff *skb,
					      struct net_device *master)
{
	if (skb->pkt_type == PACKET_HOST) {
		u16 *dest = (u16 *) eth_hdr(skb)->h_dest;

		memcpy(dest, master->dev_addr, ETH_ALEN);
	}
}
