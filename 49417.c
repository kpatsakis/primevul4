static int __sha256_ssse3_update(struct shash_desc *desc, const u8 *data,
			       unsigned int len, unsigned int partial)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);
	unsigned int done = 0;

	sctx->count += len;

	if (partial) {
		done = SHA256_BLOCK_SIZE - partial;
		memcpy(sctx->buf + partial, data, done);
		sha256_transform_asm(sctx->buf, sctx->state, 1);
	}

	if (len - done >= SHA256_BLOCK_SIZE) {
		const unsigned int rounds = (len - done) / SHA256_BLOCK_SIZE;

		sha256_transform_asm(data + done, sctx->state, (u64) rounds);

		done += rounds * SHA256_BLOCK_SIZE;
	}

	memcpy(sctx->buf, data + done, len - done);

	return 0;
}
