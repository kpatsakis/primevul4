static int md5_sparc64_update(struct shash_desc *desc, const u8 *data,
			      unsigned int len)
{
	struct md5_state *sctx = shash_desc_ctx(desc);
	unsigned int partial = sctx->byte_count % MD5_HMAC_BLOCK_SIZE;

	/* Handle the fast case right here */
	if (partial + len < MD5_HMAC_BLOCK_SIZE) {
		sctx->byte_count += len;
		memcpy((u8 *)sctx->block + partial, data, len);
	} else
		__md5_sparc64_update(sctx, data, len, partial);

	return 0;
}
