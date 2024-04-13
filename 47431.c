static int rfc4106_set_authsize(struct crypto_aead *parent,
				unsigned int authsize)
{
	struct aesni_rfc4106_gcm_ctx *ctx = aesni_rfc4106_gcm_ctx_get(parent);
	struct crypto_aead *cryptd_child = cryptd_aead_child(ctx->cryptd_tfm);

	switch (authsize) {
	case 8:
	case 12:
	case 16:
		break;
	default:
		return -EINVAL;
	}
	crypto_aead_crt(parent)->authsize = authsize;
	crypto_aead_crt(cryptd_child)->authsize = authsize;
	return 0;
}
