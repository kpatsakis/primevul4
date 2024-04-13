static int tg3_rx(struct tg3_napi *tnapi, int budget)
{
	struct tg3 *tp = tnapi->tp;
	u32 work_mask, rx_std_posted = 0;
	u32 std_prod_idx, jmb_prod_idx;
	u32 sw_idx = tnapi->rx_rcb_ptr;
	u16 hw_idx;
	int received;
	struct tg3_rx_prodring_set *tpr = &tnapi->prodring;

	hw_idx = *(tnapi->rx_rcb_prod_idx);
	/*
	 * We need to order the read of hw_idx and the read of
	 * the opaque cookie.
	 */
	rmb();
	work_mask = 0;
	received = 0;
	std_prod_idx = tpr->rx_std_prod_idx;
	jmb_prod_idx = tpr->rx_jmb_prod_idx;
	while (sw_idx != hw_idx && budget > 0) {
		struct ring_info *ri;
		struct tg3_rx_buffer_desc *desc = &tnapi->rx_rcb[sw_idx];
		unsigned int len;
		struct sk_buff *skb;
		dma_addr_t dma_addr;
		u32 opaque_key, desc_idx, *post_ptr;
		u8 *data;
		u64 tstamp = 0;

		desc_idx = desc->opaque & RXD_OPAQUE_INDEX_MASK;
		opaque_key = desc->opaque & RXD_OPAQUE_RING_MASK;
		if (opaque_key == RXD_OPAQUE_RING_STD) {
			ri = &tp->napi[0].prodring.rx_std_buffers[desc_idx];
			dma_addr = dma_unmap_addr(ri, mapping);
			data = ri->data;
			post_ptr = &std_prod_idx;
			rx_std_posted++;
		} else if (opaque_key == RXD_OPAQUE_RING_JUMBO) {
			ri = &tp->napi[0].prodring.rx_jmb_buffers[desc_idx];
			dma_addr = dma_unmap_addr(ri, mapping);
			data = ri->data;
			post_ptr = &jmb_prod_idx;
		} else
			goto next_pkt_nopost;

		work_mask |= opaque_key;

		if ((desc->err_vlan & RXD_ERR_MASK) != 0 &&
		    (desc->err_vlan != RXD_ERR_ODD_NIBBLE_RCVD_MII)) {
		drop_it:
			tg3_recycle_rx(tnapi, tpr, opaque_key,
				       desc_idx, *post_ptr);
		drop_it_no_recycle:
			/* Other statistics kept track of by card. */
			tp->rx_dropped++;
			goto next_pkt;
		}

		prefetch(data + TG3_RX_OFFSET(tp));
		len = ((desc->idx_len & RXD_LEN_MASK) >> RXD_LEN_SHIFT) -
		      ETH_FCS_LEN;

		if ((desc->type_flags & RXD_FLAG_PTPSTAT_MASK) ==
		     RXD_FLAG_PTPSTAT_PTPV1 ||
		    (desc->type_flags & RXD_FLAG_PTPSTAT_MASK) ==
		     RXD_FLAG_PTPSTAT_PTPV2) {
			tstamp = tr32(TG3_RX_TSTAMP_LSB);
			tstamp |= (u64)tr32(TG3_RX_TSTAMP_MSB) << 32;
		}

		if (len > TG3_RX_COPY_THRESH(tp)) {
			int skb_size;
			unsigned int frag_size;

			skb_size = tg3_alloc_rx_data(tp, tpr, opaque_key,
						    *post_ptr, &frag_size);
			if (skb_size < 0)
				goto drop_it;

			pci_unmap_single(tp->pdev, dma_addr, skb_size,
					 PCI_DMA_FROMDEVICE);

			skb = build_skb(data, frag_size);
			if (!skb) {
				tg3_frag_free(frag_size != 0, data);
				goto drop_it_no_recycle;
			}
			skb_reserve(skb, TG3_RX_OFFSET(tp));
			/* Ensure that the update to the data happens
			 * after the usage of the old DMA mapping.
			 */
			smp_wmb();

			ri->data = NULL;

		} else {
			tg3_recycle_rx(tnapi, tpr, opaque_key,
				       desc_idx, *post_ptr);

			skb = netdev_alloc_skb(tp->dev,
					       len + TG3_RAW_IP_ALIGN);
			if (skb == NULL)
				goto drop_it_no_recycle;

			skb_reserve(skb, TG3_RAW_IP_ALIGN);
			pci_dma_sync_single_for_cpu(tp->pdev, dma_addr, len, PCI_DMA_FROMDEVICE);
			memcpy(skb->data,
			       data + TG3_RX_OFFSET(tp),
			       len);
			pci_dma_sync_single_for_device(tp->pdev, dma_addr, len, PCI_DMA_FROMDEVICE);
		}

		skb_put(skb, len);
		if (tstamp)
			tg3_hwclock_to_timestamp(tp, tstamp,
						 skb_hwtstamps(skb));

		if ((tp->dev->features & NETIF_F_RXCSUM) &&
		    (desc->type_flags & RXD_FLAG_TCPUDP_CSUM) &&
		    (((desc->ip_tcp_csum & RXD_TCPCSUM_MASK)
		      >> RXD_TCPCSUM_SHIFT) == 0xffff))
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		else
			skb_checksum_none_assert(skb);

		skb->protocol = eth_type_trans(skb, tp->dev);

		if (len > (tp->dev->mtu + ETH_HLEN) &&
		    skb->protocol != htons(ETH_P_8021Q)) {
			dev_kfree_skb(skb);
			goto drop_it_no_recycle;
		}

		if (desc->type_flags & RXD_FLAG_VLAN &&
		    !(tp->rx_mode & RX_MODE_KEEP_VLAN_TAG))
			__vlan_hwaccel_put_tag(skb,
					       desc->err_vlan & RXD_VLAN_MASK);

		napi_gro_receive(&tnapi->napi, skb);

		received++;
		budget--;

next_pkt:
		(*post_ptr)++;

		if (unlikely(rx_std_posted >= tp->rx_std_max_post)) {
			tpr->rx_std_prod_idx = std_prod_idx &
					       tp->rx_std_ring_mask;
			tw32_rx_mbox(TG3_RX_STD_PROD_IDX_REG,
				     tpr->rx_std_prod_idx);
			work_mask &= ~RXD_OPAQUE_RING_STD;
			rx_std_posted = 0;
		}
next_pkt_nopost:
		sw_idx++;
		sw_idx &= tp->rx_ret_ring_mask;

		/* Refresh hw_idx to see if there is new work */
		if (sw_idx == hw_idx) {
			hw_idx = *(tnapi->rx_rcb_prod_idx);
			rmb();
		}
	}

	/* ACK the status ring. */
	tnapi->rx_rcb_ptr = sw_idx;
	tw32_rx_mbox(tnapi->consmbox, sw_idx);

	/* Refill RX ring(s). */
	if (!tg3_flag(tp, ENABLE_RSS)) {
		/* Sync BD data before updating mailbox */
		wmb();

		if (work_mask & RXD_OPAQUE_RING_STD) {
			tpr->rx_std_prod_idx = std_prod_idx &
					       tp->rx_std_ring_mask;
			tw32_rx_mbox(TG3_RX_STD_PROD_IDX_REG,
				     tpr->rx_std_prod_idx);
		}
		if (work_mask & RXD_OPAQUE_RING_JUMBO) {
			tpr->rx_jmb_prod_idx = jmb_prod_idx &
					       tp->rx_jmb_ring_mask;
			tw32_rx_mbox(TG3_RX_JMB_PROD_IDX_REG,
				     tpr->rx_jmb_prod_idx);
		}
		mmiowb();
	} else if (work_mask) {
		/* rx_std_buffers[] and rx_jmb_buffers[] entries must be
		 * updated before the producer indices can be updated.
		 */
		smp_wmb();

		tpr->rx_std_prod_idx = std_prod_idx & tp->rx_std_ring_mask;
		tpr->rx_jmb_prod_idx = jmb_prod_idx & tp->rx_jmb_ring_mask;

		if (tnapi != &tp->napi[1]) {
			tp->rx_refill = true;
			napi_schedule(&tp->napi[1].napi);
		}
	}

	return received;
}
