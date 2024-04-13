static int cbcmac_create(struct crypto_template *tmpl, struct rtattr **tb)
{
	struct shash_instance *inst;
	struct crypto_alg *alg;
	int err;

	err = crypto_check_attr_type(tb, CRYPTO_ALG_TYPE_SHASH);
	if (err)
		return err;

	alg = crypto_get_attr_alg(tb, CRYPTO_ALG_TYPE_CIPHER,
				  CRYPTO_ALG_TYPE_MASK);
	if (IS_ERR(alg))
		return PTR_ERR(alg);

	inst = shash_alloc_instance("cbcmac", alg);
	err = PTR_ERR(inst);
	if (IS_ERR(inst))
		goto out_put_alg;

	err = crypto_init_spawn(shash_instance_ctx(inst), alg,
				shash_crypto_instance(inst),
				CRYPTO_ALG_TYPE_MASK);
	if (err)
		goto out_free_inst;

	inst->alg.base.cra_priority = alg->cra_priority;
	inst->alg.base.cra_blocksize = 1;

	inst->alg.digestsize = alg->cra_blocksize;
	inst->alg.descsize = ALIGN(sizeof(struct cbcmac_desc_ctx),
				   alg->cra_alignmask + 1) +
			     alg->cra_blocksize;

	inst->alg.base.cra_ctxsize = sizeof(struct cbcmac_tfm_ctx);
	inst->alg.base.cra_init = cbcmac_init_tfm;
	inst->alg.base.cra_exit = cbcmac_exit_tfm;

	inst->alg.init = crypto_cbcmac_digest_init;
	inst->alg.update = crypto_cbcmac_digest_update;
	inst->alg.final = crypto_cbcmac_digest_final;
	inst->alg.setkey = crypto_cbcmac_digest_setkey;

	err = shash_register_instance(tmpl, inst);

out_free_inst:
	if (err)
		shash_free_instance(shash_crypto_instance(inst));

out_put_alg:
	crypto_mod_put(alg);
	return err;
}
