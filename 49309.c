static void encrypt_callback(void *priv, u8 *srcdst, unsigned int nbytes)
{
	const unsigned int bsize = CAST6_BLOCK_SIZE;
	struct crypt_priv *ctx = priv;
	int i;

	ctx->fpu_enabled = cast6_fpu_begin(ctx->fpu_enabled, nbytes);

	if (nbytes == bsize * CAST6_PARALLEL_BLOCKS) {
		cast6_ecb_enc_8way(ctx->ctx, srcdst, srcdst);
		return;
	}

	for (i = 0; i < nbytes / bsize; i++, srcdst += bsize)
		__cast6_encrypt(ctx->ctx, srcdst, srcdst);
}
