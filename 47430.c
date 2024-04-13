static int rfc4106_init(struct crypto_tfm *tfm)
{
	struct cryptd_aead *cryptd_tfm;
	struct aesni_rfc4106_gcm_ctx *ctx = (struct aesni_rfc4106_gcm_ctx *)
		PTR_ALIGN((u8 *)crypto_tfm_ctx(tfm), AESNI_ALIGN);
	struct crypto_aead *cryptd_child;
	struct aesni_rfc4106_gcm_ctx *child_ctx;
	cryptd_tfm = cryptd_alloc_aead("__driver-gcm-aes-aesni", 0, 0);
	if (IS_ERR(cryptd_tfm))
		return PTR_ERR(cryptd_tfm);

	cryptd_child = cryptd_aead_child(cryptd_tfm);
	child_ctx = aesni_rfc4106_gcm_ctx_get(cryptd_child);
	memcpy(child_ctx, ctx, sizeof(*ctx));
	ctx->cryptd_tfm = cryptd_tfm;
	tfm->crt_aead.reqsize = sizeof(struct aead_request)
		+ crypto_aead_reqsize(&cryptd_tfm->base);
	return 0;
}
