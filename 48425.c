static int crypto_rfc3686_init_tfm(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct crypto_skcipher_spawn *spawn = crypto_instance_ctx(inst);
	struct crypto_rfc3686_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_ablkcipher *cipher;
	unsigned long align;

	cipher = crypto_spawn_skcipher(spawn);
	if (IS_ERR(cipher))
		return PTR_ERR(cipher);

	ctx->child = cipher;

	align = crypto_tfm_alg_alignmask(tfm);
	align &= ~(crypto_tfm_ctx_alignment() - 1);
	tfm->crt_ablkcipher.reqsize = align +
		sizeof(struct crypto_rfc3686_req_ctx) +
		crypto_ablkcipher_reqsize(cipher);

	return 0;
}
