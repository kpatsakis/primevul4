static int rmd320_final(struct shash_desc *desc, u8 *out)
{
	struct rmd320_ctx *rctx = shash_desc_ctx(desc);
	u32 i, index, padlen;
	__le64 bits;
	__le32 *dst = (__le32 *)out;
	static const u8 padding[64] = { 0x80, };

	bits = cpu_to_le64(rctx->byte_count << 3);

	/* Pad out to 56 mod 64 */
	index = rctx->byte_count & 0x3f;
	padlen = (index < 56) ? (56 - index) : ((64+56) - index);
	rmd320_update(desc, padding, padlen);

	/* Append length */
	rmd320_update(desc, (const u8 *)&bits, sizeof(bits));

	/* Store state in digest */
	for (i = 0; i < 10; i++)
		dst[i] = cpu_to_le32p(&rctx->state[i]);

	/* Wipe context */
	memset(rctx, 0, sizeof(*rctx));

	return 0;
}
