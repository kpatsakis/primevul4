static void tg3_init_bufmgr_config(struct tg3 *tp)
{
	if (tg3_flag(tp, 57765_PLUS)) {
		tp->bufmgr_config.mbuf_read_dma_low_water =
			DEFAULT_MB_RDMA_LOW_WATER_5705;
		tp->bufmgr_config.mbuf_mac_rx_low_water =
			DEFAULT_MB_MACRX_LOW_WATER_57765;
		tp->bufmgr_config.mbuf_high_water =
			DEFAULT_MB_HIGH_WATER_57765;

		tp->bufmgr_config.mbuf_read_dma_low_water_jumbo =
			DEFAULT_MB_RDMA_LOW_WATER_5705;
		tp->bufmgr_config.mbuf_mac_rx_low_water_jumbo =
			DEFAULT_MB_MACRX_LOW_WATER_JUMBO_57765;
		tp->bufmgr_config.mbuf_high_water_jumbo =
			DEFAULT_MB_HIGH_WATER_JUMBO_57765;
	} else if (tg3_flag(tp, 5705_PLUS)) {
		tp->bufmgr_config.mbuf_read_dma_low_water =
			DEFAULT_MB_RDMA_LOW_WATER_5705;
		tp->bufmgr_config.mbuf_mac_rx_low_water =
			DEFAULT_MB_MACRX_LOW_WATER_5705;
		tp->bufmgr_config.mbuf_high_water =
			DEFAULT_MB_HIGH_WATER_5705;
		if (tg3_asic_rev(tp) == ASIC_REV_5906) {
			tp->bufmgr_config.mbuf_mac_rx_low_water =
				DEFAULT_MB_MACRX_LOW_WATER_5906;
			tp->bufmgr_config.mbuf_high_water =
				DEFAULT_MB_HIGH_WATER_5906;
		}

		tp->bufmgr_config.mbuf_read_dma_low_water_jumbo =
			DEFAULT_MB_RDMA_LOW_WATER_JUMBO_5780;
		tp->bufmgr_config.mbuf_mac_rx_low_water_jumbo =
			DEFAULT_MB_MACRX_LOW_WATER_JUMBO_5780;
		tp->bufmgr_config.mbuf_high_water_jumbo =
			DEFAULT_MB_HIGH_WATER_JUMBO_5780;
	} else {
		tp->bufmgr_config.mbuf_read_dma_low_water =
			DEFAULT_MB_RDMA_LOW_WATER;
		tp->bufmgr_config.mbuf_mac_rx_low_water =
			DEFAULT_MB_MACRX_LOW_WATER;
		tp->bufmgr_config.mbuf_high_water =
			DEFAULT_MB_HIGH_WATER;

		tp->bufmgr_config.mbuf_read_dma_low_water_jumbo =
			DEFAULT_MB_RDMA_LOW_WATER_JUMBO;
		tp->bufmgr_config.mbuf_mac_rx_low_water_jumbo =
			DEFAULT_MB_MACRX_LOW_WATER_JUMBO;
		tp->bufmgr_config.mbuf_high_water_jumbo =
			DEFAULT_MB_HIGH_WATER_JUMBO;
	}

	tp->bufmgr_config.dma_low_water = DEFAULT_DMA_LOW_WATER;
	tp->bufmgr_config.dma_high_water = DEFAULT_DMA_HIGH_WATER;
}
