static int rfc4106_decrypt(struct aead_request *req)
{
	int ret;
	struct crypto_aead *tfm = crypto_aead_reqtfm(req);
	struct aesni_rfc4106_gcm_ctx *ctx = aesni_rfc4106_gcm_ctx_get(tfm);

	if (!irq_fpu_usable()) {
		struct aead_request *cryptd_req =
			(struct aead_request *) aead_request_ctx(req);
		memcpy(cryptd_req, req, sizeof(*req));
		aead_request_set_tfm(cryptd_req, &ctx->cryptd_tfm->base);
		return crypto_aead_decrypt(cryptd_req);
	} else {
		struct crypto_aead *cryptd_child = cryptd_aead_child(ctx->cryptd_tfm);
		kernel_fpu_begin();
		ret = cryptd_child->base.crt_aead.decrypt(req);
		kernel_fpu_end();
		return ret;
	}
}
