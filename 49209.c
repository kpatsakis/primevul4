static int sha256_sparc64_update(struct shash_desc *desc, const u8 *data,
				 unsigned int len)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);
	unsigned int partial = sctx->count % SHA256_BLOCK_SIZE;

	/* Handle the fast case right here */
	if (partial + len < SHA256_BLOCK_SIZE) {
		sctx->count += len;
		memcpy(sctx->buf + partial, data, len);
	} else
		__sha256_sparc64_update(sctx, data, len, partial);

	return 0;
}
