static struct crypto_instance *crypto_rfc4309_alloc(struct rtattr **tb)
{
	struct crypto_attr_type *algt;
	struct crypto_instance *inst;
	struct crypto_aead_spawn *spawn;
	struct crypto_alg *alg;
	const char *ccm_name;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return ERR_CAST(algt);

	if ((algt->type ^ CRYPTO_ALG_TYPE_AEAD) & algt->mask)
		return ERR_PTR(-EINVAL);

	ccm_name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(ccm_name))
		return ERR_CAST(ccm_name);

	inst = kzalloc(sizeof(*inst) + sizeof(*spawn), GFP_KERNEL);
	if (!inst)
		return ERR_PTR(-ENOMEM);

	spawn = crypto_instance_ctx(inst);
	crypto_set_aead_spawn(spawn, inst);
	err = crypto_grab_aead(spawn, ccm_name, 0,
			       crypto_requires_sync(algt->type, algt->mask));
	if (err)
		goto out_free_inst;

	alg = crypto_aead_spawn_alg(spawn);

	err = -EINVAL;

	/* We only support 16-byte blocks. */
	if (alg->cra_aead.ivsize != 16)
		goto out_drop_alg;

	/* Not a stream cipher? */
	if (alg->cra_blocksize != 1)
		goto out_drop_alg;

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.cra_name, CRYPTO_MAX_ALG_NAME,
		     "rfc4309(%s)", alg->cra_name) >= CRYPTO_MAX_ALG_NAME ||
	    snprintf(inst->alg.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "rfc4309(%s)", alg->cra_driver_name) >=
	    CRYPTO_MAX_ALG_NAME)
		goto out_drop_alg;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_AEAD;
	inst->alg.cra_flags |= alg->cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.cra_priority = alg->cra_priority;
	inst->alg.cra_blocksize = 1;
	inst->alg.cra_alignmask = alg->cra_alignmask;
	inst->alg.cra_type = &crypto_nivaead_type;

	inst->alg.cra_aead.ivsize = 8;
	inst->alg.cra_aead.maxauthsize = 16;

	inst->alg.cra_ctxsize = sizeof(struct crypto_rfc4309_ctx);

	inst->alg.cra_init = crypto_rfc4309_init_tfm;
	inst->alg.cra_exit = crypto_rfc4309_exit_tfm;

	inst->alg.cra_aead.setkey = crypto_rfc4309_setkey;
	inst->alg.cra_aead.setauthsize = crypto_rfc4309_setauthsize;
	inst->alg.cra_aead.encrypt = crypto_rfc4309_encrypt;
	inst->alg.cra_aead.decrypt = crypto_rfc4309_decrypt;

	inst->alg.cra_aead.geniv = "seqiv";

out:
	return inst;

out_drop_alg:
	crypto_drop_aead(spawn);
out_free_inst:
	kfree(inst);
	inst = ERR_PTR(err);
	goto out;
}
