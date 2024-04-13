static void des_encrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	struct s390_des_ctx *ctx = crypto_tfm_ctx(tfm);

	crypt_s390_km(KM_DEA_ENCRYPT, ctx->key, out, in, DES_BLOCK_SIZE);
}
