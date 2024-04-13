static int crypto_rfc4543_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct crypto_rfc4543_instance_ctx *ictx = crypto_instance_ctx(inst);
	struct crypto_aead_spawn *spawn = &ictx->aead;
	struct crypto_rfc4543_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_aead *aead;
	struct crypto_blkcipher *null;
	unsigned long align;
	int err = 0;

	aead = crypto_spawn_aead(spawn);
	if (IS_ERR(aead))
		return PTR_ERR(aead);

	null = crypto_spawn_blkcipher(&ictx->null.base);
	err = PTR_ERR(null);
	if (IS_ERR(null))
		goto err_free_aead;

	ctx->child = aead;
	ctx->null = null;

	align = crypto_aead_alignmask(aead);
	align &= ~(crypto_tfm_ctx_alignment() - 1);
	tfm->crt_aead.reqsize = sizeof(struct crypto_rfc4543_req_ctx) +
				ALIGN(crypto_aead_reqsize(aead),
				      crypto_tfm_ctx_alignment()) +
				align + 16;

	return 0;

err_free_aead:
	crypto_free_aead(aead);
	return err;
}
