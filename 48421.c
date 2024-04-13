static struct crypto_instance *crypto_rfc3686_alloc(struct rtattr **tb)
{
	struct crypto_attr_type *algt;
	struct crypto_instance *inst;
	struct crypto_alg *alg;
	struct crypto_skcipher_spawn *spawn;
	const char *cipher_name;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return ERR_CAST(algt);

	if ((algt->type ^ CRYPTO_ALG_TYPE_BLKCIPHER) & algt->mask)
		return ERR_PTR(-EINVAL);

	cipher_name = crypto_attr_alg_name(tb[1]);
	if (IS_ERR(cipher_name))
		return ERR_CAST(cipher_name);

	inst = kzalloc(sizeof(*inst) + sizeof(*spawn), GFP_KERNEL);
	if (!inst)
		return ERR_PTR(-ENOMEM);

	spawn = crypto_instance_ctx(inst);

	crypto_set_skcipher_spawn(spawn, inst);
	err = crypto_grab_skcipher(spawn, cipher_name, 0,
				   crypto_requires_sync(algt->type,
							algt->mask));
	if (err)
		goto err_free_inst;

	alg = crypto_skcipher_spawn_alg(spawn);

	/* We only support 16-byte blocks. */
	err = -EINVAL;
	if (alg->cra_ablkcipher.ivsize != CTR_RFC3686_BLOCK_SIZE)
		goto err_drop_spawn;

	/* Not a stream cipher? */
	if (alg->cra_blocksize != 1)
		goto err_drop_spawn;

	err = -ENAMETOOLONG;
	if (snprintf(inst->alg.cra_name, CRYPTO_MAX_ALG_NAME, "rfc3686(%s)",
		     alg->cra_name) >= CRYPTO_MAX_ALG_NAME)
		goto err_drop_spawn;
	if (snprintf(inst->alg.cra_driver_name, CRYPTO_MAX_ALG_NAME,
		     "rfc3686(%s)", alg->cra_driver_name) >=
			CRYPTO_MAX_ALG_NAME)
		goto err_drop_spawn;

	inst->alg.cra_priority = alg->cra_priority;
	inst->alg.cra_blocksize = 1;
	inst->alg.cra_alignmask = alg->cra_alignmask;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_ABLKCIPHER |
			      (alg->cra_flags & CRYPTO_ALG_ASYNC);
	inst->alg.cra_type = &crypto_ablkcipher_type;

	inst->alg.cra_ablkcipher.ivsize = CTR_RFC3686_IV_SIZE;
	inst->alg.cra_ablkcipher.min_keysize =
		alg->cra_ablkcipher.min_keysize + CTR_RFC3686_NONCE_SIZE;
	inst->alg.cra_ablkcipher.max_keysize =
		alg->cra_ablkcipher.max_keysize + CTR_RFC3686_NONCE_SIZE;

	inst->alg.cra_ablkcipher.geniv = "seqiv";

	inst->alg.cra_ablkcipher.setkey = crypto_rfc3686_setkey;
	inst->alg.cra_ablkcipher.encrypt = crypto_rfc3686_crypt;
	inst->alg.cra_ablkcipher.decrypt = crypto_rfc3686_crypt;

	inst->alg.cra_ctxsize = sizeof(struct crypto_rfc3686_ctx);

	inst->alg.cra_init = crypto_rfc3686_init_tfm;
	inst->alg.cra_exit = crypto_rfc3686_exit_tfm;

	return inst;

err_drop_spawn:
	crypto_drop_skcipher(spawn);
err_free_inst:
	kfree(inst);
	return ERR_PTR(err);
}
