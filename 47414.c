aesni_rfc4106_gcm_ctx *aesni_rfc4106_gcm_ctx_get(struct crypto_aead *tfm)
{
	return
		(struct aesni_rfc4106_gcm_ctx *)
		PTR_ALIGN((u8 *)
		crypto_tfm_ctx(crypto_aead_tfm(tfm)), AESNI_ALIGN);
}
