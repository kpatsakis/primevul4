static int ablk_dma_crypt(struct ablkcipher_request *areq)
{
	struct crypto_ablkcipher *cipher = crypto_ablkcipher_reqtfm(areq);
	struct cryp_ctx *ctx = crypto_ablkcipher_ctx(cipher);
	struct cryp_device_data *device_data;

	int bytes_written = 0;
	int bytes_read = 0;
	int ret;

	pr_debug(DEV_DBG_NAME " [%s]", __func__);

	ctx->datalen = areq->nbytes;
	ctx->outlen = areq->nbytes;

	ret = cryp_get_device_data(ctx, &device_data);
	if (ret)
		return ret;

	ret = cryp_setup_context(ctx, device_data);
	if (ret)
		goto out;

	/* We have the device now, so store the nents in the dma struct. */
	ctx->device->dma.nents_src = get_nents(areq->src, ctx->datalen);
	ctx->device->dma.nents_dst = get_nents(areq->dst, ctx->outlen);

	/* Enable DMA in- and output. */
	cryp_configure_for_dma(device_data, CRYP_DMA_ENABLE_BOTH_DIRECTIONS);

	bytes_written = cryp_dma_write(ctx, areq->src, ctx->datalen);
	bytes_read = cryp_dma_read(ctx, areq->dst, bytes_written);

	wait_for_completion(&ctx->device->dma.cryp_dma_complete);
	cryp_dma_done(ctx);

	cryp_save_device_context(device_data, &ctx->dev_ctx, cryp_mode);
	ctx->updated = 1;

out:
	spin_lock(&device_data->ctx_lock);
	device_data->current_ctx = NULL;
	ctx->device = NULL;
	spin_unlock(&device_data->ctx_lock);

	/*
	 * The down_interruptible part for this semaphore is called in
	 * cryp_get_device_data.
	 */
	up(&driver_data.device_allocation);

	if (unlikely(bytes_written != bytes_read))
		return -EPERM;

	return 0;
}
