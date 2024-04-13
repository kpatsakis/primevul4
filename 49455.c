static void encrypt_callback(void *priv, u8 *srcdst, unsigned int nbytes)
{
	const unsigned int bsize = TF_BLOCK_SIZE;
	struct twofish_ctx *ctx = priv;
	int i;

	if (nbytes == 3 * bsize) {
		twofish_enc_blk_3way(ctx, srcdst, srcdst);
		return;
	}

	for (i = 0; i < nbytes / bsize; i++, srcdst += bsize)
		twofish_enc_blk(ctx, srcdst, srcdst);
}
