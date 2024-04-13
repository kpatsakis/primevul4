static int cryp_dma_write(struct cryp_ctx *ctx, struct scatterlist *sg,
			  int len)
{
	int error = cryp_set_dma_transfer(ctx, sg, len, DMA_TO_DEVICE);
	dev_dbg(ctx->device->dev, "[%s]: ", __func__);

	if (error) {
		dev_dbg(ctx->device->dev, "[%s]: cryp_set_dma_transfer() "
			"failed", __func__);
		return error;
	}

	return len;
}
