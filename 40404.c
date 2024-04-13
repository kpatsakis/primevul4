struct crypto_instance *aead_geniv_alloc(struct crypto_template *tmpl,
					 struct rtattr **tb, u32 type,
					 u32 mask)
{
	const char *name;
	struct crypto_aead_spawn *spawn;
	struct crypto_attr_type *algt;
	struct crypto_instance *inst;
	struct crypto_alg *alg;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return ERR_CAST(algt);

	if ((algt->type ^ (CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_GENIV)) &
	    algt->mask)
		return ERR_PTR(-EINVAL);

	name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(name))
		return ERR_CAST(name);

	inst = kzalloc(sizeof(*inst) + sizeof(*spawn), GFP_KERNEL);
	if (!inst)
		return ERR_PTR(-ENOMEM);

	spawn = crypto_instance_ctx(inst);

	/* Ignore async algorithms if necessary. */
	mask |= crypto_requires_sync(algt->type, algt->mask);

	crypto_set_aead_spawn(spawn, inst);
	err = crypto_grab_nivaead(spawn, name, type, mask);
	if (err)
		goto err_free_inst;

	alg = crypto_aead_spawn_alg(spawn);

	err = -EINVAL;
	if (!alg->cra_aead.ivsize)
		goto err_drop_alg;

	/*
	 * This is only true if we're constructing an algorithm with its
	 * default IV generator.  For the default generator we elide the
	 * template name and double-check the IV generator.
	 */
	if (algt->mask & CRYPTO_ALG_GENIV) {
		if (strcmp(tmpl->name, alg->cra_aead.geniv))
			goto err_drop_alg;

		memcpy(inst->alg.cra_name, alg->cra_name, CRYPTO_MAX_ALG_NAME);
		memcpy(inst->alg.cra_driver_name, alg->cra_driver_name,
		       CRYPTO_MAX_ALG_NAME);
	} else {
		err = -ENAMETOOLONG;
		if (snprintf(inst->alg.cra_name, CRYPTO_MAX_ALG_NAME,
			     "%s(%s)", tmpl->name, alg->cra_name) >=
		    CRYPTO_MAX_ALG_NAME)
			goto err_drop_alg;
		if (snprintf(inst->alg.cra_driver_name, CRYPTO_MAX_ALG_NAME,
			     "%s(%s)", tmpl->name, alg->cra_driver_name) >=
		    CRYPTO_MAX_ALG_NAME)
			goto err_drop_alg;
	}

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_GENIV;
	inst->alg.cra_flags |= alg->cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.cra_priority = alg->cra_priority;
	inst->alg.cra_blocksize = alg->cra_blocksize;
	inst->alg.cra_alignmask = alg->cra_alignmask;
	inst->alg.cra_type = &crypto_aead_type;

	inst->alg.cra_aead.ivsize = alg->cra_aead.ivsize;
	inst->alg.cra_aead.maxauthsize = alg->cra_aead.maxauthsize;
	inst->alg.cra_aead.geniv = alg->cra_aead.geniv;

	inst->alg.cra_aead.setkey = alg->cra_aead.setkey;
	inst->alg.cra_aead.setauthsize = alg->cra_aead.setauthsize;
	inst->alg.cra_aead.encrypt = alg->cra_aead.encrypt;
	inst->alg.cra_aead.decrypt = alg->cra_aead.decrypt;

out:
	return inst;

err_drop_alg:
	crypto_drop_aead(spawn);
err_free_inst:
	kfree(inst);
	inst = ERR_PTR(err);
	goto out;
}
