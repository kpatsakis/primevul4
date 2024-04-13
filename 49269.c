void camellia_crypt_ctr_2way(void *ctx, u128 *dst, const u128 *src, le128 *iv)
{
	be128 ctrblks[2];

	if (dst != src) {
		dst[0] = src[0];
		dst[1] = src[1];
	}

	le128_to_be128(&ctrblks[0], iv);
	le128_inc(iv);
	le128_to_be128(&ctrblks[1], iv);
	le128_inc(iv);

	camellia_enc_blk_xor_2way(ctx, (u8 *)dst, (u8 *)ctrblks);
}
