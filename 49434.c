static int sha512_ssse3_update(struct shash_desc *desc, const u8 *data,
			     unsigned int len)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);
	unsigned int partial = sctx->count[0] % SHA512_BLOCK_SIZE;
	int res;

	/* Handle the fast case right here */
	if (partial + len < SHA512_BLOCK_SIZE) {
		sctx->count[0] += len;
		if (sctx->count[0] < len)
			sctx->count[1]++;
		memcpy(sctx->buf + partial, data, len);

		return 0;
	}

	if (!irq_fpu_usable()) {
		res = crypto_sha512_update(desc, data, len);
	} else {
		kernel_fpu_begin();
		res = __sha512_ssse3_update(desc, data, len, partial);
		kernel_fpu_end();
	}

	return res;
}
