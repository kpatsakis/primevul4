static void atl2_intr_tx(struct atl2_adapter *adapter)
{
	struct net_device *netdev = adapter->netdev;
	u32 txd_read_ptr;
	u32 txs_write_ptr;
	struct tx_pkt_status *txs;
	struct tx_pkt_header *txph;
	int free_hole = 0;

	do {
		txs_write_ptr = (u32) atomic_read(&adapter->txs_write_ptr);
		txs = adapter->txs_ring + txs_write_ptr;
		if (!txs->update)
			break; /* tx stop here */

		free_hole = 1;
		txs->update = 0;

		if (++txs_write_ptr == adapter->txs_ring_size)
			txs_write_ptr = 0;
		atomic_set(&adapter->txs_write_ptr, (int)txs_write_ptr);

		txd_read_ptr = (u32) atomic_read(&adapter->txd_read_ptr);
		txph = (struct tx_pkt_header *)
			(((u8 *)adapter->txd_ring) + txd_read_ptr);

		if (txph->pkt_size != txs->pkt_size) {
			struct tx_pkt_status *old_txs = txs;
			printk(KERN_WARNING
				"%s: txs packet size not consistent with txd"
				" txd_:0x%08x, txs_:0x%08x!\n",
				adapter->netdev->name,
				*(u32 *)txph, *(u32 *)txs);
			printk(KERN_WARNING
				"txd read ptr: 0x%x\n",
				txd_read_ptr);
			txs = adapter->txs_ring + txs_write_ptr;
			printk(KERN_WARNING
				"txs-behind:0x%08x\n",
				*(u32 *)txs);
			if (txs_write_ptr < 2) {
				txs = adapter->txs_ring +
					(adapter->txs_ring_size +
					txs_write_ptr - 2);
			} else {
				txs = adapter->txs_ring + (txs_write_ptr - 2);
			}
			printk(KERN_WARNING
				"txs-before:0x%08x\n",
				*(u32 *)txs);
			txs = old_txs;
		}

		 /* 4for TPH */
		txd_read_ptr += (((u32)(txph->pkt_size) + 7) & ~3);
		if (txd_read_ptr >= adapter->txd_ring_size)
			txd_read_ptr -= adapter->txd_ring_size;

		atomic_set(&adapter->txd_read_ptr, (int)txd_read_ptr);

		/* tx statistics: */
		if (txs->ok) {
			netdev->stats.tx_bytes += txs->pkt_size;
			netdev->stats.tx_packets++;
		}
		else
			netdev->stats.tx_errors++;

		if (txs->defer)
			netdev->stats.collisions++;
		if (txs->abort_col)
			netdev->stats.tx_aborted_errors++;
		if (txs->late_col)
			netdev->stats.tx_window_errors++;
		if (txs->underun)
			netdev->stats.tx_fifo_errors++;
	} while (1);

	if (free_hole) {
		if (netif_queue_stopped(adapter->netdev) &&
			netif_carrier_ok(adapter->netdev))
			netif_wake_queue(adapter->netdev);
	}
}
