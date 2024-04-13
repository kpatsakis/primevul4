static int crypto_ccm_create_common(struct crypto_template *tmpl,
				    struct rtattr **tb,
				    const char *full_name,
				    const char *ctr_name,
				    const char *mac_name)
{
	struct crypto_attr_type *algt;
	struct aead_instance *inst;
	struct skcipher_alg *ctr;
	struct crypto_alg *mac_alg;
	struct hash_alg_common *mac;
	struct ccm_instance_ctx *ictx;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return PTR_ERR(algt);

	if ((algt->type ^ CRYPTO_ALG_TYPE_AEAD) & algt->mask)
		return -EINVAL;

	mac_alg = crypto_find_alg(mac_name, &crypto_ahash_type,
				  CRYPTO_ALG_TYPE_HASH,
				  CRYPTO_ALG_TYPE_AHASH_MASK |
				  CRYPTO_ALG_ASYNC);
	if (IS_ERR(mac_alg))
		return PTR_ERR(mac_alg);

	mac = __crypto_hash_alg_common(mac_alg);
	err = -EINVAL;
	if (mac->digestsize != 16)
		goto out_put_mac;

	inst = kzalloc(sizeof(*inst) + sizeof(*ictx), GFP_KERNEL);
	err = -ENOMEM;
	if (!inst)
		goto out_put_mac;

	ictx = aead_instance_ctx(inst);
	err = crypto_init_ahash_spawn(&ictx->mac, mac,
				      aead_crypto_instance(inst));
	if (err)
		goto err_free_inst;

	crypto_set_skcipher_spawn(&ictx->ctr, aead_crypto_instance(inst));
	err = crypto_grab_skcipher(&ictx->ctr, ctr_name, 0,
				   crypto_requires_sync(algt->type,
							algt->mask));
	if (err)
		goto err_drop_mac;

	ctr = crypto_spawn_skcipher_alg(&ictx->ctr);

	/* Not a stream cipher? */
	err = -EINVAL;
	if (ctr->base.cra_blocksize != 1)
		goto err_drop_ctr;

	/* We want the real thing! */
	if (crypto_skcipher_alg_ivsize(ctr) != 16)
		goto err_drop_ctr;

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.base.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "ccm_base(%s,%s)", ctr->base.cra_driver_name,
		     mac->base.cra_driver_name) >= CRYPTO_MAX_ALG_NAME)
		goto err_drop_ctr;

	memcpy(inst->alg.base.cra_name, full_name, CRYPTO_MAX_ALG_NAME);

	inst->alg.base.cra_flags = ctr->base.cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.base.cra_priority = (mac->base.cra_priority +
				       ctr->base.cra_priority) / 2;
	inst->alg.base.cra_blocksize = 1;
	inst->alg.base.cra_alignmask = mac->base.cra_alignmask |
				       ctr->base.cra_alignmask;
	inst->alg.ivsize = 16;
	inst->alg.chunksize = crypto_skcipher_alg_chunksize(ctr);
	inst->alg.maxauthsize = 16;
	inst->alg.base.cra_ctxsize = sizeof(struct crypto_ccm_ctx);
	inst->alg.init = crypto_ccm_init_tfm;
	inst->alg.exit = crypto_ccm_exit_tfm;
	inst->alg.setkey = crypto_ccm_setkey;
	inst->alg.setauthsize = crypto_ccm_setauthsize;
	inst->alg.encrypt = crypto_ccm_encrypt;
	inst->alg.decrypt = crypto_ccm_decrypt;

	inst->free = crypto_ccm_free;

	err = aead_register_instance(tmpl, inst);
	if (err)
		goto err_drop_ctr;

out_put_mac:
	crypto_mod_put(mac_alg);
	return err;

err_drop_ctr:
	crypto_drop_skcipher(&ictx->ctr);
err_drop_mac:
	crypto_drop_ahash(&ictx->mac);
err_free_inst:
	kfree(inst);
	goto out_put_mac;
}
