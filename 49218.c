static int sha512_sparc64_update(struct shash_desc *desc, const u8 *data,
				 unsigned int len)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);
	unsigned int partial = sctx->count[0] % SHA512_BLOCK_SIZE;

	/* Handle the fast case right here */
	if (partial + len < SHA512_BLOCK_SIZE) {
		if ((sctx->count[0] += len) < len)
			sctx->count[1]++;
		memcpy(sctx->buf + partial, data, len);
	} else
		__sha512_sparc64_update(sctx, data, len, partial);

	return 0;
}
