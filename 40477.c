struct crypto_instance *skcipher_geniv_alloc(struct crypto_template *tmpl,
					     struct rtattr **tb, u32 type,
					     u32 mask)
{
	struct {
		int (*setkey)(struct crypto_ablkcipher *tfm, const u8 *key,
			      unsigned int keylen);
		int (*encrypt)(struct ablkcipher_request *req);
		int (*decrypt)(struct ablkcipher_request *req);

		unsigned int min_keysize;
		unsigned int max_keysize;
		unsigned int ivsize;

		const char *geniv;
	} balg;
	const char *name;
	struct crypto_skcipher_spawn *spawn;
	struct crypto_attr_type *algt;
	struct crypto_instance *inst;
	struct crypto_alg *alg;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return ERR_CAST(algt);

	if ((algt->type ^ (CRYPTO_ALG_TYPE_GIVCIPHER | CRYPTO_ALG_GENIV)) &
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

	crypto_set_skcipher_spawn(spawn, inst);
	err = crypto_grab_nivcipher(spawn, name, type, mask);
	if (err)
		goto err_free_inst;

	alg = crypto_skcipher_spawn_alg(spawn);

	if ((alg->cra_flags & CRYPTO_ALG_TYPE_MASK) ==
	    CRYPTO_ALG_TYPE_BLKCIPHER) {
		balg.ivsize = alg->cra_blkcipher.ivsize;
		balg.min_keysize = alg->cra_blkcipher.min_keysize;
		balg.max_keysize = alg->cra_blkcipher.max_keysize;

		balg.setkey = async_setkey;
		balg.encrypt = async_encrypt;
		balg.decrypt = async_decrypt;

		balg.geniv = alg->cra_blkcipher.geniv;
	} else {
		balg.ivsize = alg->cra_ablkcipher.ivsize;
		balg.min_keysize = alg->cra_ablkcipher.min_keysize;
		balg.max_keysize = alg->cra_ablkcipher.max_keysize;

		balg.setkey = alg->cra_ablkcipher.setkey;
		balg.encrypt = alg->cra_ablkcipher.encrypt;
		balg.decrypt = alg->cra_ablkcipher.decrypt;

		balg.geniv = alg->cra_ablkcipher.geniv;
	}

	err = -EINVAL;
	if (!balg.ivsize)
		goto err_drop_alg;

	/*
	 * This is only true if we're constructing an algorithm with its
	 * default IV generator.  For the default generator we elide the
	 * template name and double-check the IV generator.
	 */
	if (algt->mask & CRYPTO_ALG_GENIV) {
		if (!balg.geniv)
			balg.geniv = crypto_default_geniv(alg);
		err = -EAGAIN;
		if (strcmp(tmpl->name, balg.geniv))
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

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_GIVCIPHER | CRYPTO_ALG_GENIV;
	inst->alg.cra_flags |= alg->cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.cra_priority = alg->cra_priority;
	inst->alg.cra_blocksize = alg->cra_blocksize;
	inst->alg.cra_alignmask = alg->cra_alignmask;
	inst->alg.cra_type = &crypto_givcipher_type;

	inst->alg.cra_ablkcipher.ivsize = balg.ivsize;
	inst->alg.cra_ablkcipher.min_keysize = balg.min_keysize;
	inst->alg.cra_ablkcipher.max_keysize = balg.max_keysize;
	inst->alg.cra_ablkcipher.geniv = balg.geniv;

	inst->alg.cra_ablkcipher.setkey = balg.setkey;
	inst->alg.cra_ablkcipher.encrypt = balg.encrypt;
	inst->alg.cra_ablkcipher.decrypt = balg.decrypt;

out:
	return inst;

err_drop_alg:
	crypto_drop_skcipher(spawn);
err_free_inst:
	kfree(inst);
	inst = ERR_PTR(err);
	goto out;
}
