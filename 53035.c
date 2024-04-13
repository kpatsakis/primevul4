static inline int TxdFreeBytes(struct atl2_adapter *adapter)
{
	u32 txd_read_ptr = (u32)atomic_read(&adapter->txd_read_ptr);

	return (adapter->txd_write_ptr >= txd_read_ptr) ?
		(int) (adapter->txd_ring_size - adapter->txd_write_ptr +
		txd_read_ptr - 1) :
		(int) (txd_read_ptr - adapter->txd_write_ptr - 1);
}
