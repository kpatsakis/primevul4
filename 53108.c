static netdev_tx_t atl2_xmit_frame(struct sk_buff *skb,
					 struct net_device *netdev)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	struct tx_pkt_header *txph;
	u32 offset, copy_len;
	int txs_unused;
	int txbuf_unused;

	if (test_bit(__ATL2_DOWN, &adapter->flags)) {
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	if (unlikely(skb->len <= 0)) {
		dev_kfree_skb_any(skb);
		return NETDEV_TX_OK;
	}

	txs_unused = TxsFreeUnit(adapter);
	txbuf_unused = TxdFreeBytes(adapter);

	if (skb->len + sizeof(struct tx_pkt_header) + 4  > txbuf_unused ||
		txs_unused < 1) {
		/* not enough resources */
		netif_stop_queue(netdev);
		return NETDEV_TX_BUSY;
	}

	offset = adapter->txd_write_ptr;

	txph = (struct tx_pkt_header *) (((u8 *)adapter->txd_ring) + offset);

	*(u32 *)txph = 0;
	txph->pkt_size = skb->len;

	offset += 4;
	if (offset >= adapter->txd_ring_size)
		offset -= adapter->txd_ring_size;
	copy_len = adapter->txd_ring_size - offset;
	if (copy_len >= skb->len) {
		memcpy(((u8 *)adapter->txd_ring) + offset, skb->data, skb->len);
		offset += ((u32)(skb->len + 3) & ~3);
	} else {
		memcpy(((u8 *)adapter->txd_ring)+offset, skb->data, copy_len);
		memcpy((u8 *)adapter->txd_ring, skb->data+copy_len,
			skb->len-copy_len);
		offset = ((u32)(skb->len-copy_len + 3) & ~3);
	}
#ifdef NETIF_F_HW_VLAN_CTAG_TX
	if (skb_vlan_tag_present(skb)) {
		u16 vlan_tag = skb_vlan_tag_get(skb);
		vlan_tag = (vlan_tag << 4) |
			(vlan_tag >> 13) |
			((vlan_tag >> 9) & 0x8);
		txph->ins_vlan = 1;
		txph->vlan = vlan_tag;
	}
#endif
	if (offset >= adapter->txd_ring_size)
		offset -= adapter->txd_ring_size;
	adapter->txd_write_ptr = offset;

	/* clear txs before send */
	adapter->txs_ring[adapter->txs_next_clear].update = 0;
	if (++adapter->txs_next_clear == adapter->txs_ring_size)
		adapter->txs_next_clear = 0;

	ATL2_WRITE_REGW(&adapter->hw, REG_MB_TXD_WR_IDX,
		(adapter->txd_write_ptr >> 2));

	mmiowb();
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}
