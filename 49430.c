static int sha512_ssse3_final(struct shash_desc *desc, u8 *out)
{
	struct sha512_state *sctx = shash_desc_ctx(desc);
	unsigned int i, index, padlen;
	__be64 *dst = (__be64 *)out;
	__be64 bits[2];
	static const u8 padding[SHA512_BLOCK_SIZE] = { 0x80, };

	/* save number of bits */
	bits[1] = cpu_to_be64(sctx->count[0] << 3);
	bits[0] = cpu_to_be64(sctx->count[1] << 3 | sctx->count[0] >> 61);

	/* Pad out to 112 mod 128 and append length */
	index = sctx->count[0] & 0x7f;
	padlen = (index < 112) ? (112 - index) : ((128+112) - index);

	if (!irq_fpu_usable()) {
		crypto_sha512_update(desc, padding, padlen);
		crypto_sha512_update(desc, (const u8 *)&bits, sizeof(bits));
	} else {
		kernel_fpu_begin();
		/* We need to fill a whole block for __sha512_ssse3_update() */
		if (padlen <= 112) {
			sctx->count[0] += padlen;
			if (sctx->count[0] < padlen)
				sctx->count[1]++;
			memcpy(sctx->buf + index, padding, padlen);
		} else {
			__sha512_ssse3_update(desc, padding, padlen, index);
		}
		__sha512_ssse3_update(desc, (const u8 *)&bits,
					sizeof(bits), 112);
		kernel_fpu_end();
	}

	/* Store state in digest */
	for (i = 0; i < 8; i++)
		dst[i] = cpu_to_be64(sctx->state[i]);

	/* Wipe context */
	memset(sctx, 0, sizeof(*sctx));

	return 0;
}
