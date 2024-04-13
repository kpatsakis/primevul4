static struct crypto_instance *crypto_cbc_alloc(struct rtattr **tb)
{
	struct crypto_instance *inst;
	struct crypto_alg *alg;
	int err;

	err = crypto_check_attr_type(tb, CRYPTO_ALG_TYPE_BLKCIPHER);
	if (err)
		return ERR_PTR(err);

	alg = crypto_get_attr_alg(tb, CRYPTO_ALG_TYPE_CIPHER,
				  CRYPTO_ALG_TYPE_MASK);
	if (IS_ERR(alg))
		return ERR_CAST(alg);

	inst = ERR_PTR(-EINVAL);
	if (!is_power_of_2(alg->cra_blocksize))
		goto out_put_alg;

	inst = crypto_alloc_instance("cbc", alg);
	if (IS_ERR(inst))
		goto out_put_alg;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_BLKCIPHER;
	inst->alg.cra_priority = alg->cra_priority;
	inst->alg.cra_blocksize = alg->cra_blocksize;
	inst->alg.cra_alignmask = alg->cra_alignmask;
	inst->alg.cra_type = &crypto_blkcipher_type;

	/* We access the data as u32s when xoring. */
	inst->alg.cra_alignmask |= __alignof__(u32) - 1;

	inst->alg.cra_blkcipher.ivsize = alg->cra_blocksize;
	inst->alg.cra_blkcipher.min_keysize = alg->cra_cipher.cia_min_keysize;
	inst->alg.cra_blkcipher.max_keysize = alg->cra_cipher.cia_max_keysize;

	inst->alg.cra_ctxsize = sizeof(struct crypto_cbc_ctx);

	inst->alg.cra_init = crypto_cbc_init_tfm;
	inst->alg.cra_exit = crypto_cbc_exit_tfm;

	inst->alg.cra_blkcipher.setkey = crypto_cbc_setkey;
	inst->alg.cra_blkcipher.encrypt = crypto_cbc_encrypt;
	inst->alg.cra_blkcipher.decrypt = crypto_cbc_decrypt;

out_put_alg:
	crypto_mod_put(alg);
	return inst;
}
