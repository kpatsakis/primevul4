static void init_ring_ptrs(struct atl2_adapter *adapter)
{
	/* Read / Write Ptr Initialize: */
	adapter->txd_write_ptr = 0;
	atomic_set(&adapter->txd_read_ptr, 0);

	adapter->rxd_read_ptr = 0;
	adapter->rxd_write_ptr = 0;

	atomic_set(&adapter->txs_write_ptr, 0);
	adapter->txs_next_clear = 0;
}
