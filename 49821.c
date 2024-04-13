static int cryp_cra_init(struct crypto_tfm *tfm)
{
	struct cryp_ctx *ctx = crypto_tfm_ctx(tfm);
	struct crypto_alg *alg = tfm->__crt_alg;
	struct cryp_algo_template *cryp_alg = container_of(alg,
			struct cryp_algo_template,
			crypto);

	ctx->config.algomode = cryp_alg->algomode;
	ctx->blocksize = crypto_tfm_alg_blocksize(tfm);

	return 0;
}
