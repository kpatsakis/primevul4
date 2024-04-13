static inline int TxsFreeUnit(struct atl2_adapter *adapter)
{
	u32 txs_write_ptr = (u32) atomic_read(&adapter->txs_write_ptr);

	return (adapter->txs_next_clear >= txs_write_ptr) ?
		(int) (adapter->txs_ring_size - adapter->txs_next_clear +
		txs_write_ptr - 1) :
		(int) (txs_write_ptr - adapter->txs_next_clear - 1);
}
