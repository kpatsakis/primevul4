void twofish_dec_blk_cbc_3way(void *ctx, u128 *dst, const u128 *src)
{
	u128 ivs[2];

	ivs[0] = src[0];
	ivs[1] = src[1];

	twofish_dec_blk_3way(ctx, (u8 *)dst, (u8 *)src);

	u128_xor(&dst[1], &dst[1], &ivs[0]);
	u128_xor(&dst[2], &dst[2], &ivs[1]);
}
