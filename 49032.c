static int __sha1_neon_update(struct shash_desc *desc, const u8 *data,
			       unsigned int len, unsigned int partial)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);
	unsigned int done = 0;

	sctx->count += len;

	if (partial) {
		done = SHA1_BLOCK_SIZE - partial;
		memcpy(sctx->buffer + partial, data, done);
		sha1_transform_neon(sctx->state, sctx->buffer, 1);
	}

	if (len - done >= SHA1_BLOCK_SIZE) {
		const unsigned int rounds = (len - done) / SHA1_BLOCK_SIZE;

		sha1_transform_neon(sctx->state, data + done, rounds);
		done += rounds * SHA1_BLOCK_SIZE;
	}

	memcpy(sctx->buffer, data + done, len - done);

	return 0;
}
