static int hw_crypt_noxts(struct cryp_ctx *ctx,
			  struct cryp_device_data *device_data)
{
	int ret = 0;

	const u8 *indata = ctx->indata;
	u8 *outdata = ctx->outdata;
	u32 datalen = ctx->datalen;
	u32 outlen = datalen;

	pr_debug(DEV_DBG_NAME " [%s]", __func__);

	ctx->outlen = ctx->datalen;

	if (unlikely(!IS_ALIGNED((u32)indata, 4))) {
		pr_debug(DEV_DBG_NAME " [%s]: Data isn't aligned! Addr: "
			 "0x%08x", __func__, (u32)indata);
		return -EINVAL;
	}

	ret = cryp_setup_context(ctx, device_data);

	if (ret)
		goto out;

	if (cryp_mode == CRYP_MODE_INTERRUPT) {
		cryp_enable_irq_src(device_data, CRYP_IRQ_SRC_INPUT_FIFO |
				    CRYP_IRQ_SRC_OUTPUT_FIFO);

		/*
		 * ctx->outlen is decremented in the cryp_interrupt_handler
		 * function. We had to add cpu_relax() (barrier) to make sure
		 * that gcc didn't optimze away this variable.
		 */
		while (ctx->outlen > 0)
			cpu_relax();
	} else if (cryp_mode == CRYP_MODE_POLLING ||
		   cryp_mode == CRYP_MODE_DMA) {
		/*
		 * The reason for having DMA in this if case is that if we are
		 * running cryp_mode = 2, then we separate DMA routines for
		 * handling cipher/plaintext > blocksize, except when
		 * running the normal CRYPTO_ALG_TYPE_CIPHER, then we still use
		 * the polling mode. Overhead of doing DMA setup eats up the
		 * benefits using it.
		 */
		cryp_polling_mode(ctx, device_data);
	} else {
		dev_err(ctx->device->dev, "[%s]: Invalid operation mode!",
			__func__);
		ret = -EPERM;
		goto out;
	}

	cryp_save_device_context(device_data, &ctx->dev_ctx, cryp_mode);
	ctx->updated = 1;

out:
	ctx->indata = indata;
	ctx->outdata = outdata;
	ctx->datalen = datalen;
	ctx->outlen = outlen;

	return ret;
}
