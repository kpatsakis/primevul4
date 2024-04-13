static void rfc4106_exit(struct crypto_tfm *tfm)
{
	struct aesni_rfc4106_gcm_ctx *ctx =
		(struct aesni_rfc4106_gcm_ctx *)
		PTR_ALIGN((u8 *)crypto_tfm_ctx(tfm), AESNI_ALIGN);
	if (!IS_ERR(ctx->cryptd_tfm))
		cryptd_free_aead(ctx->cryptd_tfm);
	return;
}
