static int sha1_neon_update(struct shash_desc *desc, const u8 *data,
			     unsigned int len)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);
	unsigned int partial = sctx->count % SHA1_BLOCK_SIZE;
	int res;

	/* Handle the fast case right here */
	if (partial + len < SHA1_BLOCK_SIZE) {
		sctx->count += len;
		memcpy(sctx->buffer + partial, data, len);

		return 0;
	}

	if (!may_use_simd()) {
		res = sha1_update_arm(desc, data, len);
	} else {
		kernel_neon_begin();
		res = __sha1_neon_update(desc, data, len, partial);
		kernel_neon_end();
	}

	return res;
}
