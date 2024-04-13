static int tgr192_update(struct shash_desc *desc, const u8 *inbuf,
			  unsigned int len)
{
	struct tgr192_ctx *tctx = shash_desc_ctx(desc);

	if (tctx->count == 64) {	/* flush the buffer */
		tgr192_transform(tctx, tctx->hash);
		tctx->count = 0;
		tctx->nblocks++;
	}
	if (!inbuf) {
		return 0;
	}
	if (tctx->count) {
		for (; len && tctx->count < 64; len--) {
			tctx->hash[tctx->count++] = *inbuf++;
		}
		tgr192_update(desc, NULL, 0);
		if (!len) {
			return 0;
		}

	}

	while (len >= 64) {
		tgr192_transform(tctx, inbuf);
		tctx->count = 0;
		tctx->nblocks++;
		len -= 64;
		inbuf += 64;
	}
	for (; len && tctx->count < 64; len--) {
		tctx->hash[tctx->count++] = *inbuf++;
	}

	return 0;
}
