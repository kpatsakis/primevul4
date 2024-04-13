static int crypto_rfc4309_create(struct crypto_template *tmpl,
				 struct rtattr **tb)
{
	struct crypto_attr_type *algt;
	struct aead_instance *inst;
	struct crypto_aead_spawn *spawn;
	struct aead_alg *alg;
	const char *ccm_name;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return PTR_ERR(algt);

	if ((algt->type ^ CRYPTO_ALG_TYPE_AEAD) & algt->mask)
		return -EINVAL;

	ccm_name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(ccm_name))
		return PTR_ERR(ccm_name);

	inst = kzalloc(sizeof(*inst) + sizeof(*spawn), GFP_KERNEL);
	if (!inst)
		return -ENOMEM;

	spawn = aead_instance_ctx(inst);
	crypto_set_aead_spawn(spawn, aead_crypto_instance(inst));
	err = crypto_grab_aead(spawn, ccm_name, 0,
			       crypto_requires_sync(algt->type, algt->mask));
	if (err)
		goto out_free_inst;

	alg = crypto_spawn_aead_alg(spawn);

	err = -EINVAL;

	/* We only support 16-byte blocks. */
	if (crypto_aead_alg_ivsize(alg) != 16)
		goto out_drop_alg;

	/* Not a stream cipher? */
	if (alg->base.cra_blocksize != 1)
		goto out_drop_alg;

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.base.cra_name, CRYPTO_MAX_ALG_NAME,
		     "rfc4309(%s)", alg->base.cra_name) >=
	    CRYPTO_MAX_ALG_NAME ||
	    snprintf(inst->alg.base.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "rfc4309(%s)", alg->base.cra_driver_name) >=
	    CRYPTO_MAX_ALG_NAME)
		goto out_drop_alg;

	inst->alg.base.cra_flags = alg->base.cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.base.cra_priority = alg->base.cra_priority;
	inst->alg.base.cra_blocksize = 1;
	inst->alg.base.cra_alignmask = alg->base.cra_alignmask;

	inst->alg.ivsize = 8;
	inst->alg.chunksize = crypto_aead_alg_chunksize(alg);
	inst->alg.maxauthsize = 16;

	inst->alg.base.cra_ctxsize = sizeof(struct crypto_rfc4309_ctx);

	inst->alg.init = crypto_rfc4309_init_tfm;
	inst->alg.exit = crypto_rfc4309_exit_tfm;

	inst->alg.setkey = crypto_rfc4309_setkey;
	inst->alg.setauthsize = crypto_rfc4309_setauthsize;
	inst->alg.encrypt = crypto_rfc4309_encrypt;
	inst->alg.decrypt = crypto_rfc4309_decrypt;

	inst->free = crypto_rfc4309_free;

	err = aead_register_instance(tmpl, inst);
	if (err)
		goto out_drop_alg;

out:
	return err;

out_drop_alg:
	crypto_drop_aead(spawn);
out_free_inst:
	kfree(inst);
	goto out;
}
