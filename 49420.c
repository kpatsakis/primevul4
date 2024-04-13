static int sha256_ssse3_final(struct shash_desc *desc, u8 *out)
{
	struct sha256_state *sctx = shash_desc_ctx(desc);
	unsigned int i, index, padlen;
	__be32 *dst = (__be32 *)out;
	__be64 bits;
	static const u8 padding[SHA256_BLOCK_SIZE] = { 0x80, };

	bits = cpu_to_be64(sctx->count << 3);

	/* Pad out to 56 mod 64 and append length */
	index = sctx->count % SHA256_BLOCK_SIZE;
	padlen = (index < 56) ? (56 - index) : ((SHA256_BLOCK_SIZE+56)-index);

	if (!irq_fpu_usable()) {
		crypto_sha256_update(desc, padding, padlen);
		crypto_sha256_update(desc, (const u8 *)&bits, sizeof(bits));
	} else {
		kernel_fpu_begin();
		/* We need to fill a whole block for __sha256_ssse3_update() */
		if (padlen <= 56) {
			sctx->count += padlen;
			memcpy(sctx->buf + index, padding, padlen);
		} else {
			__sha256_ssse3_update(desc, padding, padlen, index);
		}
		__sha256_ssse3_update(desc, (const u8 *)&bits,
					sizeof(bits), 56);
		kernel_fpu_end();
	}

	/* Store state in digest */
	for (i = 0; i < 8; i++)
		dst[i] = cpu_to_be32(sctx->state[i]);

	/* Wipe context */
	memset(sctx, 0, sizeof(*sctx));

	return 0;
}
