static void __sha1_sparc64_update(struct sha1_state *sctx, const u8 *data,
				  unsigned int len, unsigned int partial)
{
	unsigned int done = 0;

	sctx->count += len;
	if (partial) {
		done = SHA1_BLOCK_SIZE - partial;
		memcpy(sctx->buffer + partial, data, done);
		sha1_sparc64_transform(sctx->state, sctx->buffer, 1);
	}
	if (len - done >= SHA1_BLOCK_SIZE) {
		const unsigned int rounds = (len - done) / SHA1_BLOCK_SIZE;

		sha1_sparc64_transform(sctx->state, data + done, rounds);
		done += rounds * SHA1_BLOCK_SIZE;
	}

	memcpy(sctx->buffer, data + done, len - done);
}
