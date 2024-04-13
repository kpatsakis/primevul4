static bool tg3_tx_frag_set(struct tg3_napi *tnapi, u32 *entry, u32 *budget,
			    dma_addr_t map, u32 len, u32 flags,
			    u32 mss, u32 vlan)
{
	struct tg3 *tp = tnapi->tp;
	bool hwbug = false;

	if (tg3_flag(tp, SHORT_DMA_BUG) && len <= 8)
		hwbug = true;

	if (tg3_4g_overflow_test(map, len))
		hwbug = true;

	if (tg3_40bit_overflow_test(tp, map, len))
		hwbug = true;

	if (tp->dma_limit) {
		u32 prvidx = *entry;
		u32 tmp_flag = flags & ~TXD_FLAG_END;
		while (len > tp->dma_limit && *budget) {
			u32 frag_len = tp->dma_limit;
			len -= tp->dma_limit;

			/* Avoid the 8byte DMA problem */
			if (len <= 8) {
				len += tp->dma_limit / 2;
				frag_len = tp->dma_limit / 2;
			}

			tnapi->tx_buffers[*entry].fragmented = true;

			tg3_tx_set_bd(&tnapi->tx_ring[*entry], map,
				      frag_len, tmp_flag, mss, vlan);
			*budget -= 1;
			prvidx = *entry;
			*entry = NEXT_TX(*entry);

			map += frag_len;
		}

		if (len) {
			if (*budget) {
				tg3_tx_set_bd(&tnapi->tx_ring[*entry], map,
					      len, flags, mss, vlan);
				*budget -= 1;
				*entry = NEXT_TX(*entry);
			} else {
				hwbug = true;
				tnapi->tx_buffers[prvidx].fragmented = false;
			}
		}
	} else {
		tg3_tx_set_bd(&tnapi->tx_ring[*entry], map,
			      len, flags, mss, vlan);
		*entry = NEXT_TX(*entry);
	}

	return hwbug;
}
