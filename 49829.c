static irqreturn_t cryp_interrupt_handler(int irq, void *param)
{
	struct cryp_ctx *ctx;
	int count;
	struct cryp_device_data *device_data;

	if (param == NULL) {
		BUG_ON(!param);
		return IRQ_HANDLED;
	}

	/* The device is coming from the one found in hw_crypt_noxts. */
	device_data = (struct cryp_device_data *)param;

	ctx = device_data->current_ctx;

	if (ctx == NULL) {
		BUG_ON(!ctx);
		return IRQ_HANDLED;
	}

	dev_dbg(ctx->device->dev, "[%s] (len: %d) %s, ", __func__, ctx->outlen,
		cryp_pending_irq_src(device_data, CRYP_IRQ_SRC_OUTPUT_FIFO) ?
		"out" : "in");

	if (cryp_pending_irq_src(device_data,
				 CRYP_IRQ_SRC_OUTPUT_FIFO)) {
		if (ctx->outlen / ctx->blocksize > 0) {
			count = ctx->blocksize / 4;

			readsl(&device_data->base->dout, ctx->outdata, count);
			ctx->outdata += count;
			ctx->outlen -= count;

			if (ctx->outlen == 0) {
				cryp_disable_irq_src(device_data,
						     CRYP_IRQ_SRC_OUTPUT_FIFO);
			}
		}
	} else if (cryp_pending_irq_src(device_data,
					CRYP_IRQ_SRC_INPUT_FIFO)) {
		if (ctx->datalen / ctx->blocksize > 0) {
			count = ctx->blocksize / 4;

			writesl(&device_data->base->din, ctx->indata, count);

			ctx->indata += count;
			ctx->datalen -= count;

			if (ctx->datalen == 0)
				cryp_disable_irq_src(device_data,
						   CRYP_IRQ_SRC_INPUT_FIFO);

			if (ctx->config.algomode == CRYP_ALGO_AES_XTS) {
				CRYP_PUT_BITS(&device_data->base->cr,
					      CRYP_START_ENABLE,
					      CRYP_CR_START_POS,
					      CRYP_CR_START_MASK);

				cryp_wait_until_done(device_data);
			}
		}
	}

	return IRQ_HANDLED;
}
