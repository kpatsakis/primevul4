static int cfg_ivs(struct cryp_device_data *device_data, struct cryp_ctx *ctx)
{
	int i;
	int status = 0;
	int num_of_regs = ctx->blocksize / 8;
	u32 iv[AES_BLOCK_SIZE / 4];

	dev_dbg(device_data->dev, "[%s]", __func__);

	/*
	 * Since we loop on num_of_regs we need to have a check in case
	 * someone provides an incorrect blocksize which would force calling
	 * cfg_iv with i greater than 2 which is an error.
	 */
	if (num_of_regs > 2) {
		dev_err(device_data->dev, "[%s] Incorrect blocksize %d",
			__func__, ctx->blocksize);
		return -EINVAL;
	}

	for (i = 0; i < ctx->blocksize / 4; i++)
		iv[i] = uint8p_to_uint32_be(ctx->iv + i*4);

	for (i = 0; i < num_of_regs; i++) {
		status = cfg_iv(device_data, iv[i*2], iv[i*2+1],
				(enum cryp_init_vector_index) i);
		if (status != 0)
			return status;
	}
	return status;
}
