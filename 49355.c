static void des3_ede_x86_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	des3_ede_enc_blk(crypto_tfm_ctx(tfm), dst, src);
}
