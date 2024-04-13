static struct crypto_instance *crypto_authenc_alloc(struct rtattr **tb)
{
	struct crypto_attr_type *algt;
	struct crypto_instance *inst;
	struct hash_alg_common *auth;
	struct crypto_alg *auth_base;
	struct crypto_alg *enc;
	struct authenc_instance_ctx *ctx;
	const char *enc_name;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return ERR_CAST(algt);

	if ((algt->type ^ CRYPTO_ALG_TYPE_AEAD) & algt->mask)
		return ERR_PTR(-EINVAL);

	auth = ahash_attr_alg(tb[1], CRYPTO_ALG_TYPE_HASH,
			       CRYPTO_ALG_TYPE_AHASH_MASK);
	if (IS_ERR(auth))
		return ERR_CAST(auth);

	auth_base = &auth->base;

	enc_name = crypto_attr_alg_name(tb[2]);
	err = PTR_ERR(enc_name);
	if (IS_ERR(enc_name))
		goto out_put_auth;

	inst = kzalloc(sizeof(*inst) + sizeof(*ctx), GFP_KERNEL);
	err = -ENOMEM;
	if (!inst)
		goto out_put_auth;

	ctx = crypto_instance_ctx(inst);

	err = crypto_init_ahash_spawn(&ctx->auth, auth, inst);
	if (err)
		goto err_free_inst;

	crypto_set_skcipher_spawn(&ctx->enc, inst);
	err = crypto_grab_skcipher(&ctx->enc, enc_name, 0,
				   crypto_requires_sync(algt->type,
							algt->mask));
	if (err)
		goto err_drop_auth;

	enc = crypto_skcipher_spawn_alg(&ctx->enc);

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.cra_name, CRYPTO_MAX_ALG_NAME,
		     "authenc(%s,%s)", auth_base->cra_name, enc->cra_name) >=
	    CRYPTO_MAX_ALG_NAME)
		goto err_drop_enc;

	if (snprintf(inst->alg.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "authenc(%s,%s)", auth_base->cra_driver_name,
		     enc->cra_driver_name) >= CRYPTO_MAX_ALG_NAME)
		goto err_drop_enc;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_AEAD;
	inst->alg.cra_flags |= enc->cra_flags & CRYPTO_ALG_ASYNC;
	inst->alg.cra_priority = enc->cra_priority *
				 10 + auth_base->cra_priority;
	inst->alg.cra_blocksize = enc->cra_blocksize;
	inst->alg.cra_alignmask = auth_base->cra_alignmask | enc->cra_alignmask;
	inst->alg.cra_type = &crypto_aead_type;

	inst->alg.cra_aead.ivsize = enc->cra_ablkcipher.ivsize;
	inst->alg.cra_aead.maxauthsize = auth->digestsize;

	inst->alg.cra_ctxsize = sizeof(struct crypto_authenc_ctx);

	inst->alg.cra_init = crypto_authenc_init_tfm;
	inst->alg.cra_exit = crypto_authenc_exit_tfm;

	inst->alg.cra_aead.setkey = crypto_authenc_setkey;
	inst->alg.cra_aead.encrypt = crypto_authenc_encrypt;
	inst->alg.cra_aead.decrypt = crypto_authenc_decrypt;
	inst->alg.cra_aead.givencrypt = crypto_authenc_givencrypt;

out:
	crypto_mod_put(auth_base);
	return inst;

err_drop_enc:
	crypto_drop_skcipher(&ctx->enc);
err_drop_auth:
	crypto_drop_ahash(&ctx->auth);
err_free_inst:
	kfree(inst);
out_put_auth:
	inst = ERR_PTR(err);
	goto out;
}
