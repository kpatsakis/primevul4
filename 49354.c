static void des3_ede_x86_decrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	des3_ede_dec_blk(crypto_tfm_ctx(tfm), dst, src);
}
