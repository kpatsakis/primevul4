static void serpent_decrypt_cbc_xway(void *ctx, u128 *dst, const u128 *src)
{
	u128 ivs[SERPENT_PARALLEL_BLOCKS - 1];
	unsigned int j;

	for (j = 0; j < SERPENT_PARALLEL_BLOCKS - 1; j++)
		ivs[j] = src[j];

	serpent_dec_blk_xway(ctx, (u8 *)dst, (u8 *)src);

	for (j = 0; j < SERPENT_PARALLEL_BLOCKS - 1; j++)
		u128_xor(dst + (j + 1), dst + (j + 1), ivs + j);
}
