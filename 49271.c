static void camellia_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	camellia_enc_blk(crypto_tfm_ctx(tfm), dst, src);
}
