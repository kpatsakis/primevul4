static void blowfish_decrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	blowfish_dec_blk(crypto_tfm_ctx(tfm), dst, src);
}
