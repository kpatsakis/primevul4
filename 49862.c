static void hash_dma_setup_channel(struct hash_device_data *device_data,
				   struct device *dev)
{
	struct hash_platform_data *platform_data = dev->platform_data;
	struct dma_slave_config conf = {
		.direction = DMA_MEM_TO_DEV,
		.dst_addr = device_data->phybase + HASH_DMA_FIFO,
		.dst_addr_width = DMA_SLAVE_BUSWIDTH_2_BYTES,
		.dst_maxburst = 16,
	};

	dma_cap_zero(device_data->dma.mask);
	dma_cap_set(DMA_SLAVE, device_data->dma.mask);

	device_data->dma.cfg_mem2hash = platform_data->mem_to_engine;
	device_data->dma.chan_mem2hash =
		dma_request_channel(device_data->dma.mask,
				    platform_data->dma_filter,
				    device_data->dma.cfg_mem2hash);

	dmaengine_slave_config(device_data->dma.chan_mem2hash, &conf);

	init_completion(&device_data->dma.complete);
}
