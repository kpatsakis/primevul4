static void cryp_dma_setup_channel(struct cryp_device_data *device_data,
				   struct device *dev)
{
	struct dma_slave_config mem2cryp = {
		.direction = DMA_MEM_TO_DEV,
		.dst_addr = device_data->phybase + CRYP_DMA_TX_FIFO,
		.dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES,
		.dst_maxburst = 4,
        };
	struct dma_slave_config cryp2mem = {
		.direction = DMA_DEV_TO_MEM,
		.src_addr = device_data->phybase + CRYP_DMA_RX_FIFO,
		.src_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES,
		.src_maxburst = 4,
        };

	dma_cap_zero(device_data->dma.mask);
	dma_cap_set(DMA_SLAVE, device_data->dma.mask);

	device_data->dma.cfg_mem2cryp = mem_to_engine;
	device_data->dma.chan_mem2cryp =
		dma_request_channel(device_data->dma.mask,
				    stedma40_filter,
				    device_data->dma.cfg_mem2cryp);

	device_data->dma.cfg_cryp2mem = engine_to_mem;
	device_data->dma.chan_cryp2mem =
		dma_request_channel(device_data->dma.mask,
				    stedma40_filter,
				    device_data->dma.cfg_cryp2mem);

	dmaengine_slave_config(device_data->dma.chan_mem2cryp, &mem2cryp);
	dmaengine_slave_config(device_data->dma.chan_cryp2mem, &cryp2mem);

	init_completion(&device_data->dma.cryp_dma_complete);
}
