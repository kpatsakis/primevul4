static void lrw_xts_encrypt_callback(void *ctx, u8 *blks, unsigned int nbytes)
{
	aesni_ecb_enc(ctx, blks, blks, nbytes);
}
