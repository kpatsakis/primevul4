static struct crypto_instance *crypto_fpu_alloc(struct rtattr **tb)
{
	struct crypto_instance *inst;
	struct crypto_alg *alg;
	int err;

	err = crypto_check_attr_type(tb, CRYPTO_ALG_TYPE_BLKCIPHER);
	if (err)
		return ERR_PTR(err);

	alg = crypto_get_attr_alg(tb, CRYPTO_ALG_TYPE_BLKCIPHER,
				  CRYPTO_ALG_TYPE_MASK);
	if (IS_ERR(alg))
		return ERR_CAST(alg);

	inst = crypto_alloc_instance("fpu", alg);
	if (IS_ERR(inst))
		goto out_put_alg;

	inst->alg.cra_flags = alg->cra_flags;
	inst->alg.cra_priority = alg->cra_priority;
	inst->alg.cra_blocksize = alg->cra_blocksize;
	inst->alg.cra_alignmask = alg->cra_alignmask;
	inst->alg.cra_type = alg->cra_type;
	inst->alg.cra_blkcipher.ivsize = alg->cra_blkcipher.ivsize;
	inst->alg.cra_blkcipher.min_keysize = alg->cra_blkcipher.min_keysize;
	inst->alg.cra_blkcipher.max_keysize = alg->cra_blkcipher.max_keysize;
	inst->alg.cra_ctxsize = sizeof(struct crypto_fpu_ctx);
	inst->alg.cra_init = crypto_fpu_init_tfm;
	inst->alg.cra_exit = crypto_fpu_exit_tfm;
	inst->alg.cra_blkcipher.setkey = crypto_fpu_setkey;
	inst->alg.cra_blkcipher.encrypt = crypto_fpu_encrypt;
	inst->alg.cra_blkcipher.decrypt = crypto_fpu_decrypt;

out_put_alg:
	crypto_mod_put(alg);
	return inst;
}
