static void atl2_intr_rx(struct atl2_adapter *adapter)
{
	struct net_device *netdev = adapter->netdev;
	struct rx_desc *rxd;
	struct sk_buff *skb;

	do {
		rxd = adapter->rxd_ring+adapter->rxd_write_ptr;
		if (!rxd->status.update)
			break; /* end of tx */

		/* clear this flag at once */
		rxd->status.update = 0;

		if (rxd->status.ok && rxd->status.pkt_size >= 60) {
			int rx_size = (int)(rxd->status.pkt_size - 4);
			/* alloc new buffer */
			skb = netdev_alloc_skb_ip_align(netdev, rx_size);
			if (NULL == skb) {
				/*
				 * Check that some rx space is free. If not,
				 * free one and mark stats->rx_dropped++.
				 */
				netdev->stats.rx_dropped++;
				break;
			}
			memcpy(skb->data, rxd->packet, rx_size);
			skb_put(skb, rx_size);
			skb->protocol = eth_type_trans(skb, netdev);
			if (rxd->status.vlan) {
				u16 vlan_tag = (rxd->status.vtag>>4) |
					((rxd->status.vtag&7) << 13) |
					((rxd->status.vtag&8) << 9);

				__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vlan_tag);
			}
			netif_rx(skb);
			netdev->stats.rx_bytes += rx_size;
			netdev->stats.rx_packets++;
		} else {
			netdev->stats.rx_errors++;

			if (rxd->status.ok && rxd->status.pkt_size <= 60)
				netdev->stats.rx_length_errors++;
			if (rxd->status.mcast)
				netdev->stats.multicast++;
			if (rxd->status.crc)
				netdev->stats.rx_crc_errors++;
			if (rxd->status.align)
				netdev->stats.rx_frame_errors++;
		}

		/* advance write ptr */
		if (++adapter->rxd_write_ptr == adapter->rxd_ring_size)
			adapter->rxd_write_ptr = 0;
	} while (1);

	/* update mailbox? */
	adapter->rxd_read_ptr = adapter->rxd_write_ptr;
	ATL2_WRITE_REGW(&adapter->hw, REG_MB_RXD_RD_IDX, adapter->rxd_read_ptr);
}
