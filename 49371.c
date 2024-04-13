static int setkey(struct crypto_tfm *tfm, const u8 *key,
		  unsigned int keysize)
{
	struct salsa20_ctx *ctx = crypto_tfm_ctx(tfm);
	salsa20_keysetup(ctx, key, keysize*8, SALSA20_IV_SIZE*8);
	return 0;
}
