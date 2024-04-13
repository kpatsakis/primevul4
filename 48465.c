static int crypto_gcm_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct gcm_instance_ctx *ictx = crypto_instance_ctx(inst);
	struct crypto_gcm_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_ablkcipher *ctr;
	struct crypto_ahash *ghash;
	unsigned long align;
	int err;

	ghash = crypto_spawn_ahash(&ictx->ghash);
	if (IS_ERR(ghash))
		return PTR_ERR(ghash);

	ctr = crypto_spawn_skcipher(&ictx->ctr);
	err = PTR_ERR(ctr);
	if (IS_ERR(ctr))
		goto err_free_hash;

	ctx->ctr = ctr;
	ctx->ghash = ghash;

	align = crypto_tfm_alg_alignmask(tfm);
	align &= ~(crypto_tfm_ctx_alignment() - 1);
	tfm->crt_aead.reqsize = align +
		offsetof(struct crypto_gcm_req_priv_ctx, u) +
		max(sizeof(struct ablkcipher_request) +
		    crypto_ablkcipher_reqsize(ctr),
		    sizeof(struct ahash_request) +
		    crypto_ahash_reqsize(ghash));

	return 0;

err_free_hash:
	crypto_free_ahash(ghash);
	return err;
}
