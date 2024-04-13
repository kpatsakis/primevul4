static int cryptd_create_aead(struct crypto_template *tmpl,
		              struct rtattr **tb,
			      struct cryptd_queue *queue)
{
	struct aead_instance_ctx *ctx;
	struct crypto_instance *inst;
	struct crypto_alg *alg;
	int err;

	alg = crypto_get_attr_alg(tb, CRYPTO_ALG_TYPE_AEAD,
				CRYPTO_ALG_TYPE_MASK);
        if (IS_ERR(alg))
		return PTR_ERR(alg);

	inst = cryptd_alloc_instance(alg, 0, sizeof(*ctx));
	err = PTR_ERR(inst);
	if (IS_ERR(inst))
		goto out_put_alg;

	ctx = crypto_instance_ctx(inst);
	ctx->queue = queue;

	err = crypto_init_spawn(&ctx->aead_spawn.base, alg, inst,
			CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_ASYNC);
	if (err)
		goto out_free_inst;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC;
	inst->alg.cra_type = alg->cra_type;
	inst->alg.cra_ctxsize = sizeof(struct cryptd_aead_ctx);
	inst->alg.cra_init = cryptd_aead_init_tfm;
	inst->alg.cra_exit = cryptd_aead_exit_tfm;
	inst->alg.cra_aead.setkey      = alg->cra_aead.setkey;
	inst->alg.cra_aead.setauthsize = alg->cra_aead.setauthsize;
	inst->alg.cra_aead.geniv       = alg->cra_aead.geniv;
	inst->alg.cra_aead.ivsize      = alg->cra_aead.ivsize;
	inst->alg.cra_aead.maxauthsize = alg->cra_aead.maxauthsize;
	inst->alg.cra_aead.encrypt     = cryptd_aead_encrypt_enqueue;
	inst->alg.cra_aead.decrypt     = cryptd_aead_decrypt_enqueue;
	inst->alg.cra_aead.givencrypt  = alg->cra_aead.givencrypt;
	inst->alg.cra_aead.givdecrypt  = alg->cra_aead.givdecrypt;

	err = crypto_register_instance(tmpl, inst);
	if (err) {
		crypto_drop_spawn(&ctx->aead_spawn.base);
out_free_inst:
		kfree(inst);
	}
out_put_alg:
	crypto_mod_put(alg);
	return err;
}
