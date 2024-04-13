static int cryp_setup_context(struct cryp_ctx *ctx,
			      struct cryp_device_data *device_data)
{
	u32 control_register = CRYP_CR_DEFAULT;

	switch (cryp_mode) {
	case CRYP_MODE_INTERRUPT:
		writel_relaxed(CRYP_IMSC_DEFAULT, &device_data->base->imsc);
		break;

	case CRYP_MODE_DMA:
		writel_relaxed(CRYP_DMACR_DEFAULT, &device_data->base->dmacr);
		break;

	default:
		break;
	}

	if (ctx->updated == 0) {
		cryp_flush_inoutfifo(device_data);
		if (cfg_keys(ctx) != 0) {
			dev_err(ctx->device->dev, "[%s]: cfg_keys failed!",
				__func__);
			return -EINVAL;
		}

		if (ctx->iv &&
		    CRYP_ALGO_AES_ECB != ctx->config.algomode &&
		    CRYP_ALGO_DES_ECB != ctx->config.algomode &&
		    CRYP_ALGO_TDES_ECB != ctx->config.algomode) {
			if (cfg_ivs(device_data, ctx) != 0)
				return -EPERM;
		}

		cryp_set_configuration(device_data, &ctx->config,
				       &control_register);
		add_session_id(ctx);
	} else if (ctx->updated == 1 &&
		   ctx->session_id != atomic_read(&session_id)) {
		cryp_flush_inoutfifo(device_data);
		cryp_restore_device_context(device_data, &ctx->dev_ctx);

		add_session_id(ctx);
		control_register = ctx->dev_ctx.cr;
	} else
		control_register = ctx->dev_ctx.cr;

	writel(control_register |
	       (CRYP_CRYPEN_ENABLE << CRYP_CR_CRYPEN_POS),
	       &device_data->base->cr);

	return 0;
}
