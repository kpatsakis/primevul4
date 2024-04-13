static void aesni_xts_enc8(void *ctx, u128 *dst, const u128 *src, le128 *iv)
{
	aesni_xts_crypt8(ctx, (u8 *)dst, (const u8 *)src, true, (u8 *)iv);
}
