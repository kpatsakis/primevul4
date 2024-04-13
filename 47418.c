static void aesni_xts_tweak(void *ctx, u8 *out, const u8 *in)
{
	aesni_enc(ctx, out, in);
}
