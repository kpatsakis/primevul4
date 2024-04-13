static int hash_set_dma_transfer(struct hash_ctx *ctx, struct scatterlist *sg,
				 int len, enum dma_data_direction direction)
{
	struct dma_async_tx_descriptor *desc = NULL;
	struct dma_chan *channel = NULL;
	dma_cookie_t cookie;

	if (direction != DMA_TO_DEVICE) {
		dev_err(ctx->device->dev, "%s: Invalid DMA direction\n",
			__func__);
		return -EFAULT;
	}

	sg->length = ALIGN(sg->length, HASH_DMA_ALIGN_SIZE);

	channel = ctx->device->dma.chan_mem2hash;
	ctx->device->dma.sg = sg;
	ctx->device->dma.sg_len = dma_map_sg(channel->device->dev,
			ctx->device->dma.sg, ctx->device->dma.nents,
			direction);

	if (!ctx->device->dma.sg_len) {
		dev_err(ctx->device->dev, "%s: Could not map the sg list (TO_DEVICE)\n",
			__func__);
		return -EFAULT;
	}

	dev_dbg(ctx->device->dev, "%s: Setting up DMA for buffer (TO_DEVICE)\n",
		__func__);
	desc = dmaengine_prep_slave_sg(channel,
			ctx->device->dma.sg, ctx->device->dma.sg_len,
			direction, DMA_CTRL_ACK | DMA_PREP_INTERRUPT);
	if (!desc) {
		dev_err(ctx->device->dev,
			"%s: device_prep_slave_sg() failed!\n", __func__);
		return -EFAULT;
	}

	desc->callback = hash_dma_callback;
	desc->callback_param = ctx;

	cookie = dmaengine_submit(desc);
	dma_async_issue_pending(channel);

	return 0;
}
