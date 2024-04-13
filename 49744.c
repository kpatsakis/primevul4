static int wp512_init(struct shash_desc *desc) {
	struct wp512_ctx *wctx = shash_desc_ctx(desc);
	int i;

	memset(wctx->bitLength, 0, 32);
	wctx->bufferBits = wctx->bufferPos = 0;
	wctx->buffer[0] = 0;
	for (i = 0; i < 8; i++) {
		wctx->hash[i] = 0L;
	}

	return 0;
}
