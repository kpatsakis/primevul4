static int cryptd_create_hash(struct crypto_template *tmpl, struct rtattr **tb,
			      struct cryptd_queue *queue)
{
	struct hashd_instance_ctx *ctx;
	struct ahash_instance *inst;
	struct shash_alg *salg;
	struct crypto_alg *alg;
	int err;

	salg = shash_attr_alg(tb[1], 0, 0);
	if (IS_ERR(salg))
		return PTR_ERR(salg);

	alg = &salg->base;
	inst = cryptd_alloc_instance(alg, ahash_instance_headroom(),
				     sizeof(*ctx));
	err = PTR_ERR(inst);
	if (IS_ERR(inst))
		goto out_put_alg;

	ctx = ahash_instance_ctx(inst);
	ctx->queue = queue;

	err = crypto_init_shash_spawn(&ctx->spawn, salg,
				      ahash_crypto_instance(inst));
	if (err)
		goto out_free_inst;

	inst->alg.halg.base.cra_flags = CRYPTO_ALG_ASYNC;

	inst->alg.halg.digestsize = salg->digestsize;
	inst->alg.halg.base.cra_ctxsize = sizeof(struct cryptd_hash_ctx);

	inst->alg.halg.base.cra_init = cryptd_hash_init_tfm;
	inst->alg.halg.base.cra_exit = cryptd_hash_exit_tfm;

	inst->alg.init   = cryptd_hash_init_enqueue;
	inst->alg.update = cryptd_hash_update_enqueue;
	inst->alg.final  = cryptd_hash_final_enqueue;
	inst->alg.finup  = cryptd_hash_finup_enqueue;
	inst->alg.export = cryptd_hash_export;
	inst->alg.import = cryptd_hash_import;
	inst->alg.setkey = cryptd_hash_setkey;
	inst->alg.digest = cryptd_hash_digest_enqueue;

	err = ahash_register_instance(tmpl, inst);
	if (err) {
		crypto_drop_shash(&ctx->spawn);
out_free_inst:
		kfree(inst);
	}

out_put_alg:
	crypto_mod_put(alg);
	return err;
}
