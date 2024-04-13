int __skb_bond_should_drop(struct sk_buff *skb, struct net_device *master)
{
	struct net_device *dev = skb->dev;

	if (master->priv_flags & IFF_MASTER_ARPMON)
		dev->last_rx = jiffies;

	if ((master->priv_flags & IFF_MASTER_ALB) &&
	    (master->priv_flags & IFF_BRIDGE_PORT)) {
		/* Do address unmangle. The local destination address
		 * will be always the one master has. Provides the right
		 * functionality in a bridge.
		 */
		skb_bond_set_mac_by_master(skb, master);
	}

	if (dev->priv_flags & IFF_SLAVE_INACTIVE) {
		if ((dev->priv_flags & IFF_SLAVE_NEEDARP) &&
		    skb->protocol == __cpu_to_be16(ETH_P_ARP))
			return 0;

		if (master->priv_flags & IFF_MASTER_ALB) {
			if (skb->pkt_type != PACKET_BROADCAST &&
			    skb->pkt_type != PACKET_MULTICAST)
				return 0;
		}
		if (master->priv_flags & IFF_MASTER_8023AD &&
		    skb->protocol == __cpu_to_be16(ETH_P_SLOW))
			return 0;

		return 1;
	}
	return 0;
}
