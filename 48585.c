static struct crypto_instance *pcrypt_alloc_instance(struct crypto_alg *alg)
{
	struct crypto_instance *inst;
	struct pcrypt_instance_ctx *ctx;
	int err;

	inst = kzalloc(sizeof(*inst) + sizeof(*ctx), GFP_KERNEL);
	if (!inst) {
		inst = ERR_PTR(-ENOMEM);
		goto out;
	}

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "pcrypt(%s)", alg->cra_driver_name) >= CRYPTO_MAX_ALG_NAME)
		goto out_free_inst;

	memcpy(inst->alg.cra_name, alg->cra_name, CRYPTO_MAX_ALG_NAME);

	ctx = crypto_instance_ctx(inst);
	err = crypto_init_spawn(&ctx->spawn, alg, inst,
				CRYPTO_ALG_TYPE_MASK);
	if (err)
		goto out_free_inst;

	inst->alg.cra_priority = alg->cra_priority + 100;
	inst->alg.cra_blocksize = alg->cra_blocksize;
	inst->alg.cra_alignmask = alg->cra_alignmask;

out:
	return inst;

out_free_inst:
	kfree(inst);
	inst = ERR_PTR(err);
	goto out;
}
