static void __tg3_set_rx_mode(struct net_device *dev)
{
	struct tg3 *tp = netdev_priv(dev);
	u32 rx_mode;

	rx_mode = tp->rx_mode & ~(RX_MODE_PROMISC |
				  RX_MODE_KEEP_VLAN_TAG);

#if !defined(CONFIG_VLAN_8021Q) && !defined(CONFIG_VLAN_8021Q_MODULE)
	/* When ASF is in use, we always keep the RX_MODE_KEEP_VLAN_TAG
	 * flag clear.
	 */
	if (!tg3_flag(tp, ENABLE_ASF))
		rx_mode |= RX_MODE_KEEP_VLAN_TAG;
#endif

	if (dev->flags & IFF_PROMISC) {
		/* Promiscuous mode. */
		rx_mode |= RX_MODE_PROMISC;
	} else if (dev->flags & IFF_ALLMULTI) {
		/* Accept all multicast. */
		tg3_set_multi(tp, 1);
	} else if (netdev_mc_empty(dev)) {
		/* Reject all multicast. */
		tg3_set_multi(tp, 0);
	} else {
		/* Accept one or more multicast(s). */
		struct netdev_hw_addr *ha;
		u32 mc_filter[4] = { 0, };
		u32 regidx;
		u32 bit;
		u32 crc;

		netdev_for_each_mc_addr(ha, dev) {
			crc = calc_crc(ha->addr, ETH_ALEN);
			bit = ~crc & 0x7f;
			regidx = (bit & 0x60) >> 5;
			bit &= 0x1f;
			mc_filter[regidx] |= (1 << bit);
		}

		tw32(MAC_HASH_REG_0, mc_filter[0]);
		tw32(MAC_HASH_REG_1, mc_filter[1]);
		tw32(MAC_HASH_REG_2, mc_filter[2]);
		tw32(MAC_HASH_REG_3, mc_filter[3]);
	}

	if (rx_mode != tp->rx_mode) {
		tp->rx_mode = rx_mode;
		tw32_f(MAC_RX_MODE, rx_mode);
		udelay(10);
	}
}
