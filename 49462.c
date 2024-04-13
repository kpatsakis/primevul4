void twofish_enc_blk_ctr_3way(void *ctx, u128 *dst, const u128 *src,
			      le128 *iv)
{
	be128 ctrblks[3];

	if (dst != src) {
		dst[0] = src[0];
		dst[1] = src[1];
		dst[2] = src[2];
	}

	le128_to_be128(&ctrblks[0], iv);
	le128_inc(iv);
	le128_to_be128(&ctrblks[1], iv);
	le128_inc(iv);
	le128_to_be128(&ctrblks[2], iv);
	le128_inc(iv);

	twofish_enc_blk_xor_3way(ctx, (u8 *)dst, (u8 *)ctrblks);
}
