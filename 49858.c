static int hash_cra_init(struct crypto_tfm *tfm)
{
	struct hash_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_alg *alg = tfm->__crt_alg;
	struct hash_algo_template *hash_alg;

	hash_alg = container_of(__crypto_ahash_alg(alg),
			struct hash_algo_template,
			hash);

	crypto_ahash_set_reqsize(__crypto_ahash_cast(tfm),
				 sizeof(struct hash_req_ctx));

	ctx->config.data_format = HASH_DATA_8_BITS;
	ctx->config.algorithm = hash_alg->conf.algorithm;
	ctx->config.oper_mode = hash_alg->conf.oper_mode;

	ctx->digestsize = hash_alg->hash.halg.digestsize;

	return 0;
}
