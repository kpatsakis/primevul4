void *crypto_alloc_instance2(const char *name, struct crypto_alg *alg,
			     unsigned int head)
{
	struct crypto_instance *inst;
	char *p;
	int err;

	p = kzalloc(head + sizeof(*inst) + sizeof(struct crypto_spawn),
		    GFP_KERNEL);
	if (!p)
		return ERR_PTR(-ENOMEM);

	inst = (void *)(p + head);

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.cra_name, CRYPTO_MAX_ALG_NAME, "%s(%s)", name,
		     alg->cra_name) >= CRYPTO_MAX_ALG_NAME)
		goto err_free_inst;

	if (snprintf(inst->alg.cra_driver_name, CRYPTO_MAX_ALG_NAME, "%s(%s)",
		     name, alg->cra_driver_name) >= CRYPTO_MAX_ALG_NAME)
		goto err_free_inst;

	return p;

err_free_inst:
	kfree(p);
	return ERR_PTR(err);
}
