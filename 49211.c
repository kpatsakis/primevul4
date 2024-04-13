static void __sha512_sparc64_update(struct sha512_state *sctx, const u8 *data,
				    unsigned int len, unsigned int partial)
{
	unsigned int done = 0;

	if ((sctx->count[0] += len) < len)
		sctx->count[1]++;
	if (partial) {
		done = SHA512_BLOCK_SIZE - partial;
		memcpy(sctx->buf + partial, data, done);
		sha512_sparc64_transform(sctx->state, sctx->buf, 1);
	}
	if (len - done >= SHA512_BLOCK_SIZE) {
		const unsigned int rounds = (len - done) / SHA512_BLOCK_SIZE;

		sha512_sparc64_transform(sctx->state, data + done, rounds);
		done += rounds * SHA512_BLOCK_SIZE;
	}

	memcpy(sctx->buf, data + done, len - done);
}
