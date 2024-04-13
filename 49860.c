static void hash_dma_done(struct hash_ctx *ctx)
{
	struct dma_chan *chan;

	chan = ctx->device->dma.chan_mem2hash;
	dmaengine_device_control(chan, DMA_TERMINATE_ALL, 0);
	dma_unmap_sg(chan->device->dev, ctx->device->dma.sg,
		     ctx->device->dma.sg_len, DMA_TO_DEVICE);
}
