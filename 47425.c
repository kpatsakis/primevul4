static void lrw_xts_decrypt_callback(void *ctx, u8 *blks, unsigned int nbytes)
{
	aesni_ecb_dec(ctx, blks, blks, nbytes);
}
