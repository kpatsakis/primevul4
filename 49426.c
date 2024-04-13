static int __sha512_ssse3_update(struct shash_desc *desc, const u8 *data,
			       unsigned int len, unsigned int partial)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);
	unsigned int done = 0;

	sctx->count[0] += len;
	if (sctx->count[0] < len)
		sctx->count[1]++;

	if (partial) {
		done = SHA512_BLOCK_SIZE - partial;
		memcpy(sctx->buf + partial, data, done);
		sha512_transform_asm(sctx->buf, sctx->state, 1);
	}

	if (len - done >= SHA512_BLOCK_SIZE) {
		const unsigned int rounds = (len - done) / SHA512_BLOCK_SIZE;

		sha512_transform_asm(data + done, sctx->state, (u64) rounds);

		done += rounds * SHA512_BLOCK_SIZE;
	}

	memcpy(sctx->buf, data + done, len - done);

	return 0;
}
