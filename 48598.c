static int seqiv_aead_givencrypt_first(struct aead_givcrypt_request *req)
{
	struct crypto_aead *geniv = aead_givcrypt_reqtfm(req);
	struct seqiv_ctx *ctx = crypto_aead_ctx(geniv);
	int err = 0;

	spin_lock_bh(&ctx->lock);
	if (crypto_aead_crt(geniv)->givencrypt != seqiv_aead_givencrypt_first)
		goto unlock;

	crypto_aead_crt(geniv)->givencrypt = seqiv_aead_givencrypt;
	err = crypto_rng_get_bytes(crypto_default_rng, ctx->salt,
				   crypto_aead_ivsize(geniv));

unlock:
	spin_unlock_bh(&ctx->lock);

	if (err)
		return err;

	return seqiv_aead_givencrypt(req);
}
