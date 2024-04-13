static int crypto_gcm_setkey(struct crypto_aead *aead, const u8 *key,
			     unsigned int keylen)
{
	struct crypto_gcm_ctx *ctx = crypto_aead_ctx(aead);
	struct crypto_ahash *ghash = ctx->ghash;
	struct crypto_ablkcipher *ctr = ctx->ctr;
	struct {
		be128 hash;
		u8 iv[8];

		struct crypto_gcm_setkey_result result;

		struct scatterlist sg[1];
		struct ablkcipher_request req;
	} *data;
	int err;

	crypto_ablkcipher_clear_flags(ctr, CRYPTO_TFM_REQ_MASK);
	crypto_ablkcipher_set_flags(ctr, crypto_aead_get_flags(aead) &
				   CRYPTO_TFM_REQ_MASK);

	err = crypto_ablkcipher_setkey(ctr, key, keylen);
	if (err)
		return err;

	crypto_aead_set_flags(aead, crypto_ablkcipher_get_flags(ctr) &
				       CRYPTO_TFM_RES_MASK);

	data = kzalloc(sizeof(*data) + crypto_ablkcipher_reqsize(ctr),
		       GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	init_completion(&data->result.completion);
	sg_init_one(data->sg, &data->hash, sizeof(data->hash));
	ablkcipher_request_set_tfm(&data->req, ctr);
	ablkcipher_request_set_callback(&data->req, CRYPTO_TFM_REQ_MAY_SLEEP |
						    CRYPTO_TFM_REQ_MAY_BACKLOG,
					crypto_gcm_setkey_done,
					&data->result);
	ablkcipher_request_set_crypt(&data->req, data->sg, data->sg,
				     sizeof(data->hash), data->iv);

	err = crypto_ablkcipher_encrypt(&data->req);
	if (err == -EINPROGRESS || err == -EBUSY) {
		err = wait_for_completion_interruptible(
			&data->result.completion);
		if (!err)
			err = data->result.err;
	}

	if (err)
		goto out;

	crypto_ahash_clear_flags(ghash, CRYPTO_TFM_REQ_MASK);
	crypto_ahash_set_flags(ghash, crypto_aead_get_flags(aead) &
			       CRYPTO_TFM_REQ_MASK);
	err = crypto_ahash_setkey(ghash, (u8 *)&data->hash, sizeof(be128));
	crypto_aead_set_flags(aead, crypto_ahash_get_flags(ghash) &
			      CRYPTO_TFM_RES_MASK);

out:
	kfree(data);
	return err;
}
