static struct crypto_instance *crypto_ctr_alloc(struct rtattr **tb)
{
	struct crypto_instance *inst;
	struct crypto_alg *alg;
	int err;

	err = crypto_check_attr_type(tb, CRYPTO_ALG_TYPE_BLKCIPHER);
	if (err)
		return ERR_PTR(err);

	alg = crypto_attr_alg(tb[1], CRYPTO_ALG_TYPE_CIPHER,
				  CRYPTO_ALG_TYPE_MASK);
	if (IS_ERR(alg))
		return ERR_CAST(alg);

	/* Block size must be >= 4 bytes. */
	err = -EINVAL;
	if (alg->cra_blocksize < 4)
		goto out_put_alg;

	/* If this is false we'd fail the alignment of crypto_inc. */
	if (alg->cra_blocksize % 4)
		goto out_put_alg;

	inst = crypto_alloc_instance("ctr", alg);
	if (IS_ERR(inst))
		goto out;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_BLKCIPHER;
	inst->alg.cra_priority = alg->cra_priority;
	inst->alg.cra_blocksize = 1;
	inst->alg.cra_alignmask = alg->cra_alignmask | (__alignof__(u32) - 1);
	inst->alg.cra_type = &crypto_blkcipher_type;

	inst->alg.cra_blkcipher.ivsize = alg->cra_blocksize;
	inst->alg.cra_blkcipher.min_keysize = alg->cra_cipher.cia_min_keysize;
	inst->alg.cra_blkcipher.max_keysize = alg->cra_cipher.cia_max_keysize;

	inst->alg.cra_ctxsize = sizeof(struct crypto_ctr_ctx);

	inst->alg.cra_init = crypto_ctr_init_tfm;
	inst->alg.cra_exit = crypto_ctr_exit_tfm;

	inst->alg.cra_blkcipher.setkey = crypto_ctr_setkey;
	inst->alg.cra_blkcipher.encrypt = crypto_ctr_crypt;
	inst->alg.cra_blkcipher.decrypt = crypto_ctr_crypt;

	inst->alg.cra_blkcipher.geniv = "chainiv";

out:
	crypto_mod_put(alg);
	return inst;

out_put_alg:
	inst = ERR_PTR(err);
	goto out;
}
