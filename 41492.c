static int tg3_run_loopback(struct tg3 *tp, u32 pktsz, bool tso_loopback)
{
	u32 rx_start_idx, rx_idx, tx_idx, opaque_key;
	u32 base_flags = 0, mss = 0, desc_idx, coal_now, data_off, val;
	u32 budget;
	struct sk_buff *skb;
	u8 *tx_data, *rx_data;
	dma_addr_t map;
	int num_pkts, tx_len, rx_len, i, err;
	struct tg3_rx_buffer_desc *desc;
	struct tg3_napi *tnapi, *rnapi;
	struct tg3_rx_prodring_set *tpr = &tp->napi[0].prodring;

	tnapi = &tp->napi[0];
	rnapi = &tp->napi[0];
	if (tp->irq_cnt > 1) {
		if (tg3_flag(tp, ENABLE_RSS))
			rnapi = &tp->napi[1];
		if (tg3_flag(tp, ENABLE_TSS))
			tnapi = &tp->napi[1];
	}
	coal_now = tnapi->coal_now | rnapi->coal_now;

	err = -EIO;

	tx_len = pktsz;
	skb = netdev_alloc_skb(tp->dev, tx_len);
	if (!skb)
		return -ENOMEM;

	tx_data = skb_put(skb, tx_len);
	memcpy(tx_data, tp->dev->dev_addr, 6);
	memset(tx_data + 6, 0x0, 8);

	tw32(MAC_RX_MTU_SIZE, tx_len + ETH_FCS_LEN);

	if (tso_loopback) {
		struct iphdr *iph = (struct iphdr *)&tx_data[ETH_HLEN];

		u32 hdr_len = TG3_TSO_IP_HDR_LEN + TG3_TSO_TCP_HDR_LEN +
			      TG3_TSO_TCP_OPT_LEN;

		memcpy(tx_data + ETH_ALEN * 2, tg3_tso_header,
		       sizeof(tg3_tso_header));
		mss = TG3_TSO_MSS;

		val = tx_len - ETH_ALEN * 2 - sizeof(tg3_tso_header);
		num_pkts = DIV_ROUND_UP(val, TG3_TSO_MSS);

		/* Set the total length field in the IP header */
		iph->tot_len = htons((u16)(mss + hdr_len));

		base_flags = (TXD_FLAG_CPU_PRE_DMA |
			      TXD_FLAG_CPU_POST_DMA);

		if (tg3_flag(tp, HW_TSO_1) ||
		    tg3_flag(tp, HW_TSO_2) ||
		    tg3_flag(tp, HW_TSO_3)) {
			struct tcphdr *th;
			val = ETH_HLEN + TG3_TSO_IP_HDR_LEN;
			th = (struct tcphdr *)&tx_data[val];
			th->check = 0;
		} else
			base_flags |= TXD_FLAG_TCPUDP_CSUM;

		if (tg3_flag(tp, HW_TSO_3)) {
			mss |= (hdr_len & 0xc) << 12;
			if (hdr_len & 0x10)
				base_flags |= 0x00000010;
			base_flags |= (hdr_len & 0x3e0) << 5;
		} else if (tg3_flag(tp, HW_TSO_2))
			mss |= hdr_len << 9;
		else if (tg3_flag(tp, HW_TSO_1) ||
			 tg3_asic_rev(tp) == ASIC_REV_5705) {
			mss |= (TG3_TSO_TCP_OPT_LEN << 9);
		} else {
			base_flags |= (TG3_TSO_TCP_OPT_LEN << 10);
		}

		data_off = ETH_ALEN * 2 + sizeof(tg3_tso_header);
	} else {
		num_pkts = 1;
		data_off = ETH_HLEN;

		if (tg3_flag(tp, USE_JUMBO_BDFLAG) &&
		    tx_len > VLAN_ETH_FRAME_LEN)
			base_flags |= TXD_FLAG_JMB_PKT;
	}

	for (i = data_off; i < tx_len; i++)
		tx_data[i] = (u8) (i & 0xff);

	map = pci_map_single(tp->pdev, skb->data, tx_len, PCI_DMA_TODEVICE);
	if (pci_dma_mapping_error(tp->pdev, map)) {
		dev_kfree_skb(skb);
		return -EIO;
	}

	val = tnapi->tx_prod;
	tnapi->tx_buffers[val].skb = skb;
	dma_unmap_addr_set(&tnapi->tx_buffers[val], mapping, map);

	tw32_f(HOSTCC_MODE, tp->coalesce_mode | HOSTCC_MODE_ENABLE |
	       rnapi->coal_now);

	udelay(10);

	rx_start_idx = rnapi->hw_status->idx[0].rx_producer;

	budget = tg3_tx_avail(tnapi);
	if (tg3_tx_frag_set(tnapi, &val, &budget, map, tx_len,
			    base_flags | TXD_FLAG_END, mss, 0)) {
		tnapi->tx_buffers[val].skb = NULL;
		dev_kfree_skb(skb);
		return -EIO;
	}

	tnapi->tx_prod++;

	/* Sync BD data before updating mailbox */
	wmb();

	tw32_tx_mbox(tnapi->prodmbox, tnapi->tx_prod);
	tr32_mailbox(tnapi->prodmbox);

	udelay(10);

	/* 350 usec to allow enough time on some 10/100 Mbps devices.  */
	for (i = 0; i < 35; i++) {
		tw32_f(HOSTCC_MODE, tp->coalesce_mode | HOSTCC_MODE_ENABLE |
		       coal_now);

		udelay(10);

		tx_idx = tnapi->hw_status->idx[0].tx_consumer;
		rx_idx = rnapi->hw_status->idx[0].rx_producer;
		if ((tx_idx == tnapi->tx_prod) &&
		    (rx_idx == (rx_start_idx + num_pkts)))
			break;
	}

	tg3_tx_skb_unmap(tnapi, tnapi->tx_prod - 1, -1);
	dev_kfree_skb(skb);

	if (tx_idx != tnapi->tx_prod)
		goto out;

	if (rx_idx != rx_start_idx + num_pkts)
		goto out;

	val = data_off;
	while (rx_idx != rx_start_idx) {
		desc = &rnapi->rx_rcb[rx_start_idx++];
		desc_idx = desc->opaque & RXD_OPAQUE_INDEX_MASK;
		opaque_key = desc->opaque & RXD_OPAQUE_RING_MASK;

		if ((desc->err_vlan & RXD_ERR_MASK) != 0 &&
		    (desc->err_vlan != RXD_ERR_ODD_NIBBLE_RCVD_MII))
			goto out;

		rx_len = ((desc->idx_len & RXD_LEN_MASK) >> RXD_LEN_SHIFT)
			 - ETH_FCS_LEN;

		if (!tso_loopback) {
			if (rx_len != tx_len)
				goto out;

			if (pktsz <= TG3_RX_STD_DMA_SZ - ETH_FCS_LEN) {
				if (opaque_key != RXD_OPAQUE_RING_STD)
					goto out;
			} else {
				if (opaque_key != RXD_OPAQUE_RING_JUMBO)
					goto out;
			}
		} else if ((desc->type_flags & RXD_FLAG_TCPUDP_CSUM) &&
			   (desc->ip_tcp_csum & RXD_TCPCSUM_MASK)
			    >> RXD_TCPCSUM_SHIFT != 0xffff) {
			goto out;
		}

		if (opaque_key == RXD_OPAQUE_RING_STD) {
			rx_data = tpr->rx_std_buffers[desc_idx].data;
			map = dma_unmap_addr(&tpr->rx_std_buffers[desc_idx],
					     mapping);
		} else if (opaque_key == RXD_OPAQUE_RING_JUMBO) {
			rx_data = tpr->rx_jmb_buffers[desc_idx].data;
			map = dma_unmap_addr(&tpr->rx_jmb_buffers[desc_idx],
					     mapping);
		} else
			goto out;

		pci_dma_sync_single_for_cpu(tp->pdev, map, rx_len,
					    PCI_DMA_FROMDEVICE);

		rx_data += TG3_RX_OFFSET(tp);
		for (i = data_off; i < rx_len; i++, val++) {
			if (*(rx_data + i) != (u8) (val & 0xff))
				goto out;
		}
	}

	err = 0;

	/* tg3_free_rings will unmap and free the rx_data */
out:
	return err;
}
