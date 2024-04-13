static netdev_tx_t tg3_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct tg3 *tp = netdev_priv(dev);
	u32 len, entry, base_flags, mss, vlan = 0;
	u32 budget;
	int i = -1, would_hit_hwbug;
	dma_addr_t mapping;
	struct tg3_napi *tnapi;
	struct netdev_queue *txq;
	unsigned int last;

	txq = netdev_get_tx_queue(dev, skb_get_queue_mapping(skb));
	tnapi = &tp->napi[skb_get_queue_mapping(skb)];
	if (tg3_flag(tp, ENABLE_TSS))
		tnapi++;

	budget = tg3_tx_avail(tnapi);

	/* We are running in BH disabled context with netif_tx_lock
	 * and TX reclaim runs via tp->napi.poll inside of a software
	 * interrupt.  Furthermore, IRQ processing runs lockless so we have
	 * no IRQ context deadlocks to worry about either.  Rejoice!
	 */
	if (unlikely(budget <= (skb_shinfo(skb)->nr_frags + 1))) {
		if (!netif_tx_queue_stopped(txq)) {
			netif_tx_stop_queue(txq);

			/* This is a hard error, log it. */
			netdev_err(dev,
				   "BUG! Tx Ring full when queue awake!\n");
		}
		return NETDEV_TX_BUSY;
	}

	entry = tnapi->tx_prod;
	base_flags = 0;
	if (skb->ip_summed == CHECKSUM_PARTIAL)
		base_flags |= TXD_FLAG_TCPUDP_CSUM;

	mss = skb_shinfo(skb)->gso_size;
	if (mss) {
		struct iphdr *iph;
		u32 tcp_opt_len, hdr_len;

		if (skb_header_cloned(skb) &&
		    pskb_expand_head(skb, 0, 0, GFP_ATOMIC))
			goto drop;

		iph = ip_hdr(skb);
		tcp_opt_len = tcp_optlen(skb);

		hdr_len = skb_transport_offset(skb) + tcp_hdrlen(skb) - ETH_HLEN;

		if (!skb_is_gso_v6(skb)) {
			iph->check = 0;
			iph->tot_len = htons(mss + hdr_len);
		}

		if (unlikely((ETH_HLEN + hdr_len) > 80) &&
		    tg3_flag(tp, TSO_BUG))
			return tg3_tso_bug(tp, skb);

		base_flags |= (TXD_FLAG_CPU_PRE_DMA |
			       TXD_FLAG_CPU_POST_DMA);

		if (tg3_flag(tp, HW_TSO_1) ||
		    tg3_flag(tp, HW_TSO_2) ||
		    tg3_flag(tp, HW_TSO_3)) {
			tcp_hdr(skb)->check = 0;
			base_flags &= ~TXD_FLAG_TCPUDP_CSUM;
		} else
			tcp_hdr(skb)->check = ~csum_tcpudp_magic(iph->saddr,
								 iph->daddr, 0,
								 IPPROTO_TCP,
								 0);

		if (tg3_flag(tp, HW_TSO_3)) {
			mss |= (hdr_len & 0xc) << 12;
			if (hdr_len & 0x10)
				base_flags |= 0x00000010;
			base_flags |= (hdr_len & 0x3e0) << 5;
		} else if (tg3_flag(tp, HW_TSO_2))
			mss |= hdr_len << 9;
		else if (tg3_flag(tp, HW_TSO_1) ||
			 tg3_asic_rev(tp) == ASIC_REV_5705) {
			if (tcp_opt_len || iph->ihl > 5) {
				int tsflags;

				tsflags = (iph->ihl - 5) + (tcp_opt_len >> 2);
				mss |= (tsflags << 11);
			}
		} else {
			if (tcp_opt_len || iph->ihl > 5) {
				int tsflags;

				tsflags = (iph->ihl - 5) + (tcp_opt_len >> 2);
				base_flags |= tsflags << 12;
			}
		}
	}

	if (tg3_flag(tp, USE_JUMBO_BDFLAG) &&
	    !mss && skb->len > VLAN_ETH_FRAME_LEN)
		base_flags |= TXD_FLAG_JMB_PKT;

	if (vlan_tx_tag_present(skb)) {
		base_flags |= TXD_FLAG_VLAN;
		vlan = vlan_tx_tag_get(skb);
	}

	if ((unlikely(skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP)) &&
	    tg3_flag(tp, TX_TSTAMP_EN)) {
		skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;
		base_flags |= TXD_FLAG_HWTSTAMP;
	}

	len = skb_headlen(skb);

	mapping = pci_map_single(tp->pdev, skb->data, len, PCI_DMA_TODEVICE);
	if (pci_dma_mapping_error(tp->pdev, mapping))
		goto drop;


	tnapi->tx_buffers[entry].skb = skb;
	dma_unmap_addr_set(&tnapi->tx_buffers[entry], mapping, mapping);

	would_hit_hwbug = 0;

	if (tg3_flag(tp, 5701_DMA_BUG))
		would_hit_hwbug = 1;

	if (tg3_tx_frag_set(tnapi, &entry, &budget, mapping, len, base_flags |
			  ((skb_shinfo(skb)->nr_frags == 0) ? TXD_FLAG_END : 0),
			    mss, vlan)) {
		would_hit_hwbug = 1;
	} else if (skb_shinfo(skb)->nr_frags > 0) {
		u32 tmp_mss = mss;

		if (!tg3_flag(tp, HW_TSO_1) &&
		    !tg3_flag(tp, HW_TSO_2) &&
		    !tg3_flag(tp, HW_TSO_3))
			tmp_mss = 0;

		/* Now loop through additional data
		 * fragments, and queue them.
		 */
		last = skb_shinfo(skb)->nr_frags - 1;
		for (i = 0; i <= last; i++) {
			skb_frag_t *frag = &skb_shinfo(skb)->frags[i];

			len = skb_frag_size(frag);
			mapping = skb_frag_dma_map(&tp->pdev->dev, frag, 0,
						   len, DMA_TO_DEVICE);

			tnapi->tx_buffers[entry].skb = NULL;
			dma_unmap_addr_set(&tnapi->tx_buffers[entry], mapping,
					   mapping);
			if (dma_mapping_error(&tp->pdev->dev, mapping))
				goto dma_error;

			if (!budget ||
			    tg3_tx_frag_set(tnapi, &entry, &budget, mapping,
					    len, base_flags |
					    ((i == last) ? TXD_FLAG_END : 0),
					    tmp_mss, vlan)) {
				would_hit_hwbug = 1;
				break;
			}
		}
	}

	if (would_hit_hwbug) {
		tg3_tx_skb_unmap(tnapi, tnapi->tx_prod, i);

		/* If the workaround fails due to memory/mapping
		 * failure, silently drop this packet.
		 */
		entry = tnapi->tx_prod;
		budget = tg3_tx_avail(tnapi);
		if (tigon3_dma_hwbug_workaround(tnapi, &skb, &entry, &budget,
						base_flags, mss, vlan))
			goto drop_nofree;
	}

	skb_tx_timestamp(skb);
	netdev_tx_sent_queue(txq, skb->len);

	/* Sync BD data before updating mailbox */
	wmb();

	/* Packets are ready, update Tx producer idx local and on card. */
	tw32_tx_mbox(tnapi->prodmbox, entry);

	tnapi->tx_prod = entry;
	if (unlikely(tg3_tx_avail(tnapi) <= (MAX_SKB_FRAGS + 1))) {
		netif_tx_stop_queue(txq);

		/* netif_tx_stop_queue() must be done before checking
		 * checking tx index in tg3_tx_avail() below, because in
		 * tg3_tx(), we update tx index before checking for
		 * netif_tx_queue_stopped().
		 */
		smp_mb();
		if (tg3_tx_avail(tnapi) > TG3_TX_WAKEUP_THRESH(tnapi))
			netif_tx_wake_queue(txq);
	}

	mmiowb();
	return NETDEV_TX_OK;

dma_error:
	tg3_tx_skb_unmap(tnapi, tnapi->tx_prod, --i);
	tnapi->tx_buffers[tnapi->tx_prod].skb = NULL;
drop:
	dev_kfree_skb(skb);
drop_nofree:
	tp->tx_dropped++;
	return NETDEV_TX_OK;
}
