static void aes_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	aes_enc_blk(crypto_tfm_ctx(tfm), dst, src);
}
