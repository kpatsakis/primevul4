static void twofish_decrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	twofish_dec_blk(crypto_tfm_ctx(tfm), dst, src);
}
