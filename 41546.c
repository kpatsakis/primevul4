static inline u32 tg3_tx_avail(struct tg3_napi *tnapi)
{
	/* Tell compiler to fetch tx indices from memory. */
	barrier();
	return tnapi->tx_pending -
	       ((tnapi->tx_prod - tnapi->tx_cons) & (TG3_TX_RING_SIZE - 1));
}
