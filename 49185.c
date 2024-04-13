static void __md5_sparc64_update(struct md5_state *sctx, const u8 *data,
				 unsigned int len, unsigned int partial)
{
	unsigned int done = 0;

	sctx->byte_count += len;
	if (partial) {
		done = MD5_HMAC_BLOCK_SIZE - partial;
		memcpy((u8 *)sctx->block + partial, data, done);
		md5_sparc64_transform(sctx->hash, (u8 *)sctx->block, 1);
	}
	if (len - done >= MD5_HMAC_BLOCK_SIZE) {
		const unsigned int rounds = (len - done) / MD5_HMAC_BLOCK_SIZE;

		md5_sparc64_transform(sctx->hash, data + done, rounds);
		done += rounds * MD5_HMAC_BLOCK_SIZE;
	}

	memcpy(sctx->block, data + done, len - done);
}
