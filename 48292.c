static int crypto_authenc_esn_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = crypto_tfm_alg_instance(tfm);
	struct authenc_esn_instance_ctx *ictx = crypto_instance_ctx(inst);
	struct crypto_authenc_esn_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_ahash *auth;
	struct crypto_ablkcipher *enc;
	int err;

	auth = crypto_spawn_ahash(&ictx->auth);
	if (IS_ERR(auth))
		return PTR_ERR(auth);

	enc = crypto_spawn_skcipher(&ictx->enc);
	err = PTR_ERR(enc);
	if (IS_ERR(enc))
		goto err_free_ahash;

	ctx->auth = auth;
	ctx->enc = enc;

	ctx->reqoff = ALIGN(2 * crypto_ahash_digestsize(auth) +
			    crypto_ahash_alignmask(auth),
			    crypto_ahash_alignmask(auth) + 1) +
		      crypto_ablkcipher_ivsize(enc);

	tfm->crt_aead.reqsize = sizeof(struct authenc_esn_request_ctx) +
				ctx->reqoff +
				max_t(unsigned int,
				crypto_ahash_reqsize(auth) +
				sizeof(struct ahash_request),
				sizeof(struct skcipher_givcrypt_request) +
				crypto_ablkcipher_reqsize(enc));

	return 0;

err_free_ahash:
	crypto_free_ahash(auth);
	return err;
}
