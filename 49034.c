static int sha1_neon_final(struct shash_desc *desc, u8 *out)
{
	struct sha1_state *sctx = shash_desc_ctx(desc);
	unsigned int i, index, padlen;
	__be32 *dst = (__be32 *)out;
	__be64 bits;
	static const u8 padding[SHA1_BLOCK_SIZE] = { 0x80, };

	bits = cpu_to_be64(sctx->count << 3);

	/* Pad out to 56 mod 64 and append length */
	index = sctx->count % SHA1_BLOCK_SIZE;
	padlen = (index < 56) ? (56 - index) : ((SHA1_BLOCK_SIZE+56) - index);
	if (!may_use_simd()) {
		sha1_update_arm(desc, padding, padlen);
		sha1_update_arm(desc, (const u8 *)&bits, sizeof(bits));
	} else {
		kernel_neon_begin();
		/* We need to fill a whole block for __sha1_neon_update() */
		if (padlen <= 56) {
			sctx->count += padlen;
			memcpy(sctx->buffer + index, padding, padlen);
		} else {
			__sha1_neon_update(desc, padding, padlen, index);
		}
		__sha1_neon_update(desc, (const u8 *)&bits, sizeof(bits), 56);
		kernel_neon_end();
	}

	/* Store state in digest */
	for (i = 0; i < 5; i++)
		dst[i] = cpu_to_be32(sctx->state[i]);

	/* Wipe context */
	memset(sctx, 0, sizeof(*sctx));

	return 0;
}
