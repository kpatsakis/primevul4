static int md5_sparc64_final(struct shash_desc *desc, u8 *out)
{
	struct md5_state *sctx = shash_desc_ctx(desc);
	unsigned int i, index, padlen;
	u32 *dst = (u32 *)out;
	__le64 bits;
	static const u8 padding[MD5_HMAC_BLOCK_SIZE] = { 0x80, };

	bits = cpu_to_le64(sctx->byte_count << 3);

	/* Pad out to 56 mod 64 and append length */
	index = sctx->byte_count % MD5_HMAC_BLOCK_SIZE;
	padlen = (index < 56) ? (56 - index) : ((MD5_HMAC_BLOCK_SIZE+56) - index);

	/* We need to fill a whole block for __md5_sparc64_update() */
	if (padlen <= 56) {
		sctx->byte_count += padlen;
		memcpy((u8 *)sctx->block + index, padding, padlen);
	} else {
		__md5_sparc64_update(sctx, padding, padlen, index);
	}
	__md5_sparc64_update(sctx, (const u8 *)&bits, sizeof(bits), 56);

	/* Store state in digest */
	for (i = 0; i < MD5_HASH_WORDS; i++)
		dst[i] = sctx->hash[i];

	/* Wipe context */
	memset(sctx, 0, sizeof(*sctx));

	return 0;
}
