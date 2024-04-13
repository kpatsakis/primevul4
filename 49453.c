static void twofish_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	twofish_enc_blk(crypto_tfm_ctx(tfm), dst, src);
}
