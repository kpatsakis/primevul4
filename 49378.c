static void encrypt_callback(void *priv, u8 *srcdst, unsigned int nbytes)
{
	const unsigned int bsize = SERPENT_BLOCK_SIZE;
	struct crypt_priv *ctx = priv;
	int i;

	ctx->fpu_enabled = serpent_fpu_begin(ctx->fpu_enabled, nbytes);

	if (nbytes >= SERPENT_AVX2_PARALLEL_BLOCKS * bsize) {
		serpent_ecb_enc_16way(ctx->ctx, srcdst, srcdst);
		srcdst += bsize * SERPENT_AVX2_PARALLEL_BLOCKS;
		nbytes -= bsize * SERPENT_AVX2_PARALLEL_BLOCKS;
	}

	while (nbytes >= SERPENT_PARALLEL_BLOCKS * bsize) {
		serpent_ecb_enc_8way_avx(ctx->ctx, srcdst, srcdst);
		srcdst += bsize * SERPENT_PARALLEL_BLOCKS;
		nbytes -= bsize * SERPENT_PARALLEL_BLOCKS;
	}

	for (i = 0; i < nbytes / bsize; i++, srcdst += bsize)
		__serpent_encrypt(ctx->ctx, srcdst, srcdst);
}
