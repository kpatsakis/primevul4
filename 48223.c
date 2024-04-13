struct crypto_instance *crypto_alloc_instance(const char *name,
					      struct crypto_alg *alg)
{
	struct crypto_instance *inst;
	struct crypto_spawn *spawn;
	int err;

	inst = crypto_alloc_instance2(name, alg, 0);
	if (IS_ERR(inst))
		goto out;

	spawn = crypto_instance_ctx(inst);
	err = crypto_init_spawn(spawn, alg, inst,
				CRYPTO_ALG_TYPE_MASK | CRYPTO_ALG_ASYNC);

	if (err)
		goto err_free_inst;

	return inst;

err_free_inst:
	kfree(inst);
	inst = ERR_PTR(err);

out:
	return inst;
}
