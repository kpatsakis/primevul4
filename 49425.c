static int sha256_ssse3_update(struct shash_desc *desc, const u8 *data,
			     unsigned int len)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);
	unsigned int partial = sctx->count % SHA256_BLOCK_SIZE;
	int res;

	/* Handle the fast case right here */
	if (partial + len < SHA256_BLOCK_SIZE) {
		sctx->count += len;
		memcpy(sctx->buf + partial, data, len);

		return 0;
	}

	if (!irq_fpu_usable()) {
		res = crypto_sha256_update(desc, data, len);
	} else {
		kernel_fpu_begin();
		res = __sha256_ssse3_update(desc, data, len, partial);
		kernel_fpu_end();
	}

	return res;
}
