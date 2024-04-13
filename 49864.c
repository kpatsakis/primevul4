static int hash_dma_write(struct hash_ctx *ctx,
			  struct scatterlist *sg, int len)
{
	int error = hash_set_dma_transfer(ctx, sg, len, DMA_TO_DEVICE);
	if (error) {
		dev_dbg(ctx->device->dev,
			"%s: hash_set_dma_transfer() failed\n", __func__);
		return error;
	}

	return len;
}
