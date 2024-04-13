static struct crypto_instance *crypto_ccm_alloc_common(struct rtattr **tb,
						       const char *full_name,
						       const char *ctr_name,
						       const char *cipher_name)
{
	struct crypto_attr_type *algt;
	struct crypto_instance *inst;
	struct crypto_alg *ctr;
	struct crypto_alg *cipher;
	struct ccm_instance_ctx *ictx;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return ERR_CAST(algt);

	if ((algt->type ^ CRYPTO_ALG_TYPE_AEAD) & algt->mask)
		return ERR_PTR(-EINVAL);

	cipher = crypto_alg_mod_lookup(cipher_name,  CRYPTO_ALG_TYPE_CIPHER,
				       CRYPTO_ALG_TYPE_MASK);
	if (IS_ERR(cipher))
		return ERR_CAST(cipher);

	err = -EINVAL;
	if (cipher->cra_blocksize != 16)
		goto out_put_cipher;

	inst = kzalloc(sizeof(*inst) + sizeof(*ictx), GFP_KERNEL);
	err = -ENOMEM;
	if (!inst)
		goto out_put_cipher;

	ictx = crypto_instance_ctx(inst);

	err = crypto_init_spawn(&ictx->cipher, cipher, inst,
				CRYPTO_ALG_TYPE_MASK);
	if (err)
		goto err_free_inst;

	crypto_set_skcipher_spawn(&ictx->ctr, inst);
	err = crypto_grab_skcipher(&ictx->ctr, ctr_name, 0,
				   crypto_requires_sync(algt->type,
							algt->mask));
	if (err)
		goto err_drop_cipher;

	ctr = crypto_skcipher_spawn_alg(&ictx->ctr);

	/* Not a stream cipher? */
	err = -EINVAL;
	if (ctr->cra_blocksize != 1)
		goto err_drop_ctr;

	/* We want the real thing! */
	if (ctr->cra_ablkcipher.ivsize != 16)
		goto err_drop_ctr;

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "ccm_base(%s,%s)", ctr->cra_driver_name,
		     cipher->cra_driver_name) >= CRYPTO_MAX_ALG_NAME)
		goto err_drop_ctr;

	memcpy(inst->alg.cra_name, full_name, CRYPTO_MAX_ALG_NAME);

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_AEAD;
	inst->alg.cra_flags |= ctr->cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.cra_priority = cipher->cra_priority + ctr->cra_priority;
	inst->alg.cra_blocksize = 1;
	inst->alg.cra_alignmask = cipher->cra_alignmask | ctr->cra_alignmask |
				  (__alignof__(u32) - 1);
	inst->alg.cra_type = &crypto_aead_type;
	inst->alg.cra_aead.ivsize = 16;
	inst->alg.cra_aead.maxauthsize = 16;
	inst->alg.cra_ctxsize = sizeof(struct crypto_ccm_ctx);
	inst->alg.cra_init = crypto_ccm_init_tfm;
	inst->alg.cra_exit = crypto_ccm_exit_tfm;
	inst->alg.cra_aead.setkey = crypto_ccm_setkey;
	inst->alg.cra_aead.setauthsize = crypto_ccm_setauthsize;
	inst->alg.cra_aead.encrypt = crypto_ccm_encrypt;
	inst->alg.cra_aead.decrypt = crypto_ccm_decrypt;

out:
	crypto_mod_put(cipher);
	return inst;

err_drop_ctr:
	crypto_drop_skcipher(&ictx->ctr);
err_drop_cipher:
	crypto_drop_spawn(&ictx->cipher);
err_free_inst:
	kfree(inst);
out_put_cipher:
	inst = ERR_PTR(err);
	goto out;
}
