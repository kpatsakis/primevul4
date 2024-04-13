static int sha1_update(struct shash_desc *desc, const u8 *data,
			unsigned int len)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);
	unsigned int partial, done;
	const u8 *src;

	partial = sctx->count & 0x3f;
	sctx->count += len;
	done = 0;
	src = data;

	if ((partial + len) > 63) {
		u32 temp[SHA_WORKSPACE_WORDS];

		if (partial) {
			done = -partial;
			memcpy(sctx->buffer + partial, data, done + 64);
			src = sctx->buffer;
		}

		do {
			powerpc_sha_transform(sctx->state, src, temp);
			done += 64;
			src = data + done;
		} while (done + 63 < len);

		memset(temp, 0, sizeof(temp));
		partial = 0;
	}
	memcpy(sctx->buffer + partial, src, len - done);

	return 0;
}
