static int cryptd_create_blkcipher(struct crypto_template *tmpl,
				   struct rtattr **tb,
				   struct cryptd_queue *queue)
{
	struct cryptd_instance_ctx *ctx;
	struct crypto_instance *inst;
	struct crypto_alg *alg;
	int err;

	alg = crypto_get_attr_alg(tb, CRYPTO_ALG_TYPE_BLKCIPHER,
				  CRYPTO_ALG_TYPE_MASK);
	if (IS_ERR(alg))
		return PTR_ERR(alg);

	inst = cryptd_alloc_instance(alg, 0, sizeof(*ctx));
	err = PTR_ERR(inst);
	if (IS_ERR(inst))
		goto out_put_alg;

	ctx = crypto_instance_ctx(inst);
	ctx->queue = queue;

	err = crypto_init_spawn(&ctx->spawn, alg, inst,
				CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_ASYNC);
	if (err)
		goto out_free_inst;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER | CRYPTO_ALG_ASYNC;
	inst->alg.cra_type = &crypto_ablkcipher_type;

	inst->alg.cra_ablkcipher.ivsize = alg->cra_blkcipher.ivsize;
	inst->alg.cra_ablkcipher.min_keysize = alg->cra_blkcipher.min_keysize;
	inst->alg.cra_ablkcipher.max_keysize = alg->cra_blkcipher.max_keysize;

	inst->alg.cra_ablkcipher.geniv = alg->cra_blkcipher.geniv;

	inst->alg.cra_ctxsize = sizeof(struct cryptd_blkcipher_ctx);

	inst->alg.cra_init = cryptd_blkcipher_init_tfm;
	inst->alg.cra_exit = cryptd_blkcipher_exit_tfm;

	inst->alg.cra_ablkcipher.setkey = cryptd_blkcipher_setkey;
	inst->alg.cra_ablkcipher.encrypt = cryptd_blkcipher_encrypt_enqueue;
	inst->alg.cra_ablkcipher.decrypt = cryptd_blkcipher_decrypt_enqueue;

	err = crypto_register_instance(tmpl, inst);
	if (err) {
		crypto_drop_spawn(&ctx->spawn);
out_free_inst:
		kfree(inst);
	}

out_put_alg:
	crypto_mod_put(alg);
	return err;
}
