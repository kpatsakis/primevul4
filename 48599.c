static int seqiv_aead_init(struct crypto_tfm *tfm)
{
	struct crypto_aead *geniv = __crypto_aead_cast(tfm);
	struct seqiv_ctx *ctx = crypto_aead_ctx(geniv);

	spin_lock_init(&ctx->lock);

	tfm->crt_aead.reqsize = sizeof(struct aead_request);

	return aead_geniv_init(tfm);
}
