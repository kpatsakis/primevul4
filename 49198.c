static int sha1_sparc64_update(struct shash_desc *desc, const u8 *data,
			       unsigned int len)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);
	unsigned int partial = sctx->count % SHA1_BLOCK_SIZE;

	/* Handle the fast case right here */
	if (partial + len < SHA1_BLOCK_SIZE) {
		sctx->count += len;
		memcpy(sctx->buffer + partial, data, len);
	} else
		__sha1_sparc64_update(sctx, data, len, partial);

	return 0;
}
