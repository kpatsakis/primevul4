static void cryp_dma_out_callback(void *data)
{
	struct cryp_ctx *ctx = (struct cryp_ctx *) data;
	dev_dbg(ctx->device->dev, "[%s]: ", __func__);

	complete(&ctx->device->dma.cryp_dma_complete);
}
