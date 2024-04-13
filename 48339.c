static int async_chainiv_givencrypt_first(struct skcipher_givcrypt_request *req)
{
	struct crypto_ablkcipher *geniv = skcipher_givcrypt_reqtfm(req);
	struct async_chainiv_ctx *ctx = crypto_ablkcipher_ctx(geniv);
	int err = 0;

	if (test_and_set_bit(CHAINIV_STATE_INUSE, &ctx->state))
		goto out;

	if (crypto_ablkcipher_crt(geniv)->givencrypt !=
	    async_chainiv_givencrypt_first)
		goto unlock;

	crypto_ablkcipher_crt(geniv)->givencrypt = async_chainiv_givencrypt;
	err = crypto_rng_get_bytes(crypto_default_rng, ctx->iv,
				   crypto_ablkcipher_ivsize(geniv));

unlock:
	clear_bit(CHAINIV_STATE_INUSE, &ctx->state);

	if (err)
		return err;

out:
	return async_chainiv_givencrypt(req);
}
