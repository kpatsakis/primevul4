static int cryp_set_dma_transfer(struct cryp_ctx *ctx,
				 struct scatterlist *sg,
				 int len,
				 enum dma_data_direction direction)
{
	struct dma_async_tx_descriptor *desc;
	struct dma_chan *channel = NULL;
	dma_cookie_t cookie;

	dev_dbg(ctx->device->dev, "[%s]: ", __func__);

	if (unlikely(!IS_ALIGNED((u32)sg, 4))) {
		dev_err(ctx->device->dev, "[%s]: Data in sg list isn't "
			"aligned! Addr: 0x%08x", __func__, (u32)sg);
		return -EFAULT;
	}

	switch (direction) {
	case DMA_TO_DEVICE:
		channel = ctx->device->dma.chan_mem2cryp;
		ctx->device->dma.sg_src = sg;
		ctx->device->dma.sg_src_len = dma_map_sg(channel->device->dev,
						 ctx->device->dma.sg_src,
						 ctx->device->dma.nents_src,
						 direction);

		if (!ctx->device->dma.sg_src_len) {
			dev_dbg(ctx->device->dev,
				"[%s]: Could not map the sg list (TO_DEVICE)",
				__func__);
			return -EFAULT;
		}

		dev_dbg(ctx->device->dev, "[%s]: Setting up DMA for buffer "
			"(TO_DEVICE)", __func__);

		desc = dmaengine_prep_slave_sg(channel,
				ctx->device->dma.sg_src,
				ctx->device->dma.sg_src_len,
				direction, DMA_CTRL_ACK);
		break;

	case DMA_FROM_DEVICE:
		channel = ctx->device->dma.chan_cryp2mem;
		ctx->device->dma.sg_dst = sg;
		ctx->device->dma.sg_dst_len = dma_map_sg(channel->device->dev,
						 ctx->device->dma.sg_dst,
						 ctx->device->dma.nents_dst,
						 direction);

		if (!ctx->device->dma.sg_dst_len) {
			dev_dbg(ctx->device->dev,
				"[%s]: Could not map the sg list (FROM_DEVICE)",
				__func__);
			return -EFAULT;
		}

		dev_dbg(ctx->device->dev, "[%s]: Setting up DMA for buffer "
			"(FROM_DEVICE)", __func__);

		desc = dmaengine_prep_slave_sg(channel,
				ctx->device->dma.sg_dst,
				ctx->device->dma.sg_dst_len,
				direction,
				DMA_CTRL_ACK |
				DMA_PREP_INTERRUPT);

		desc->callback = cryp_dma_out_callback;
		desc->callback_param = ctx;
		break;

	default:
		dev_dbg(ctx->device->dev, "[%s]: Invalid DMA direction",
			__func__);
		return -EFAULT;
	}

	cookie = dmaengine_submit(desc);
	dma_async_issue_pending(channel);

	return 0;
}
