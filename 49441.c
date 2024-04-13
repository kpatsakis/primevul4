static void encrypt_callback(void *priv, u8 *srcdst, unsigned int nbytes)
{
	const unsigned int bsize = TF_BLOCK_SIZE;
	struct crypt_priv *ctx = priv;
	int i;

	ctx->fpu_enabled = twofish_fpu_begin(ctx->fpu_enabled, nbytes);

	if (nbytes == bsize * TWOFISH_PARALLEL_BLOCKS) {
		twofish_ecb_enc_8way(ctx->ctx, srcdst, srcdst);
		return;
	}

	for (i = 0; i < nbytes / (bsize * 3); i++, srcdst += bsize * 3)
		twofish_enc_blk_3way(ctx->ctx, srcdst, srcdst);

	nbytes %= bsize * 3;

	for (i = 0; i < nbytes / bsize; i++, srcdst += bsize)
		twofish_enc_blk(ctx->ctx, srcdst, srcdst);
}
