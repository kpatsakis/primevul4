static int cfg_keys(struct cryp_ctx *ctx)
{
	int i;
	int num_of_regs = ctx->keylen / 8;
	u32 swapped_key[CRYP_MAX_KEY_SIZE / 4];
	int cryp_error = 0;

	dev_dbg(ctx->device->dev, "[%s]", __func__);

	if (mode_is_aes(ctx->config.algomode)) {
		swap_words_in_key_and_bits_in_byte((u8 *)ctx->key,
						   (u8 *)swapped_key,
						   ctx->keylen);
	} else {
		for (i = 0; i < ctx->keylen / 4; i++)
			swapped_key[i] = uint8p_to_uint32_be(ctx->key + i*4);
	}

	for (i = 0; i < num_of_regs; i++) {
		cryp_error = set_key(ctx->device,
				     *(((u32 *)swapped_key)+i*2),
				     *(((u32 *)swapped_key)+i*2+1),
				     (enum cryp_key_reg_index) i);

		if (cryp_error != 0) {
			dev_err(ctx->device->dev, "[%s]: set_key() failed!",
					__func__);
			return cryp_error;
		}
	}
	return cryp_error;
}
