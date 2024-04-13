static void serpent_crypt_ctr_xway(void *ctx, u128 *dst, const u128 *src,
				   le128 *iv)
{
	be128 ctrblks[SERPENT_PARALLEL_BLOCKS];
	unsigned int i;

	for (i = 0; i < SERPENT_PARALLEL_BLOCKS; i++) {
		if (dst != src)
			dst[i] = src[i];

		le128_to_be128(&ctrblks[i], iv);
		le128_inc(iv);
	}

	serpent_enc_blk_xway_xor(ctx, (u8 *)dst, (u8 *)ctrblks);
}
