static void catc_set_multicast_list(struct net_device *netdev)
{
	struct catc *catc = netdev_priv(netdev);
	struct netdev_hw_addr *ha;
	u8 broadcast[ETH_ALEN];
	u8 rx = RxEnable | RxPolarity | RxMultiCast;

	eth_broadcast_addr(broadcast);
	memset(catc->multicast, 0, 64);

	catc_multicast(broadcast, catc->multicast);
	catc_multicast(netdev->dev_addr, catc->multicast);

	if (netdev->flags & IFF_PROMISC) {
		memset(catc->multicast, 0xff, 64);
		rx |= (!catc->is_f5u011) ? RxPromisc : AltRxPromisc;
	} 

	if (netdev->flags & IFF_ALLMULTI) {
		memset(catc->multicast, 0xff, 64);
	} else {
		netdev_for_each_mc_addr(ha, netdev) {
			u32 crc = ether_crc_le(6, ha->addr);
			if (!catc->is_f5u011) {
				catc->multicast[(crc >> 3) & 0x3f] |= 1 << (crc & 7);
			} else {
				catc->multicast[7-(crc >> 29)] |= 1 << ((crc >> 26) & 7);
			}
		}
	}
	if (!catc->is_f5u011) {
		catc_set_reg_async(catc, RxUnit, rx);
		catc_write_mem_async(catc, 0xfa80, catc->multicast, 64);
	} else {
		f5u011_mchash_async(catc, catc->multicast);
		if (catc->rxmode[0] != rx) {
			catc->rxmode[0] = rx;
			netdev_dbg(catc->netdev,
				   "Setting RX mode to %2.2X %2.2X\n",
				   catc->rxmode[0], catc->rxmode[1]);
			f5u011_rxmode_async(catc, catc->rxmode);
		}
	}
}
