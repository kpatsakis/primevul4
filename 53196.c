trad_enc_init(struct trad_enc_ctx *ctx, const char *pw, size_t pw_len,
    const uint8_t *key, size_t key_len, uint8_t *crcchk)
{
	uint8_t header[12];

	if (key_len < 12) {
		*crcchk = 0xff;
		return -1;
	}

	ctx->keys[0] = 305419896L;
	ctx->keys[1] = 591751049L;
	ctx->keys[2] = 878082192L;

	for (;pw_len; --pw_len)
		trad_enc_update_keys(ctx, *pw++);

	trad_enc_decrypt_update(ctx, key, 12, header, 12);
	/* Return the last byte for CRC check. */
	*crcchk = header[11];
	return 0;
}
