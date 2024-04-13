static void encrypt_callback(void *priv, u8 *srcdst, unsigned int nbytes)
{
	const unsigned int bsize = CAMELLIA_BLOCK_SIZE;
	struct crypt_priv *ctx = priv;
	int i;

	ctx->fpu_enabled = camellia_fpu_begin(ctx->fpu_enabled, nbytes);

	if (nbytes >= CAMELLIA_AESNI_PARALLEL_BLOCKS * bsize) {
		camellia_ecb_enc_16way(ctx->ctx, srcdst, srcdst);
		srcdst += bsize * CAMELLIA_AESNI_PARALLEL_BLOCKS;
		nbytes -= bsize * CAMELLIA_AESNI_PARALLEL_BLOCKS;
	}

	while (nbytes >= CAMELLIA_PARALLEL_BLOCKS * bsize) {
		camellia_enc_blk_2way(ctx->ctx, srcdst, srcdst);
		srcdst += bsize * CAMELLIA_PARALLEL_BLOCKS;
		nbytes -= bsize * CAMELLIA_PARALLEL_BLOCKS;
	}

	for (i = 0; i < nbytes / bsize; i++, srcdst += bsize)
		camellia_enc_blk(ctx->ctx, srcdst, srcdst);
}
