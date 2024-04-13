static int eseqiv_givencrypt_first(struct skcipher_givcrypt_request *req)
{
	struct crypto_ablkcipher *geniv = skcipher_givcrypt_reqtfm(req);
	struct eseqiv_ctx *ctx = crypto_ablkcipher_ctx(geniv);
	int err = 0;

	spin_lock_bh(&ctx->lock);
	if (crypto_ablkcipher_crt(geniv)->givencrypt != eseqiv_givencrypt_first)
		goto unlock;

	crypto_ablkcipher_crt(geniv)->givencrypt = eseqiv_givencrypt;
	err = crypto_rng_get_bytes(crypto_default_rng, ctx->salt,
				   crypto_ablkcipher_ivsize(geniv));

unlock:
	spin_unlock_bh(&ctx->lock);

	if (err)
		return err;

	return eseqiv_givencrypt(req);
}
