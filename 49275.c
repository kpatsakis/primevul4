static void decrypt_callback(void *priv, u8 *srcdst, unsigned int nbytes)
{
	const unsigned int bsize = CAMELLIA_BLOCK_SIZE;
	struct camellia_ctx *ctx = priv;
	int i;

	while (nbytes >= 2 * bsize) {
		camellia_dec_blk_2way(ctx, srcdst, srcdst);
		srcdst += bsize * 2;
		nbytes -= bsize * 2;
	}

	for (i = 0; i < nbytes / bsize; i++, srcdst += bsize)
		camellia_dec_blk(ctx, srcdst, srcdst);
}
