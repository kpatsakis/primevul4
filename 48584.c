static struct crypto_instance *pcrypt_alloc_aead(struct rtattr **tb,
						 u32 type, u32 mask)
{
	struct crypto_instance *inst;
	struct crypto_alg *alg;

	alg = crypto_get_attr_alg(tb, type, (mask & CRYPTO_ALG_TYPE_MASK));
	if (IS_ERR(alg))
		return ERR_CAST(alg);

	inst = pcrypt_alloc_instance(alg);
	if (IS_ERR(inst))
		goto out_put_alg;

	inst->alg.cra_flags = CRYPTO_ALG_TYPE_AEAD | CRYPTO_ALG_ASYNC;
	inst->alg.cra_type = &crypto_aead_type;

	inst->alg.cra_aead.ivsize = alg->cra_aead.ivsize;
	inst->alg.cra_aead.geniv = alg->cra_aead.geniv;
	inst->alg.cra_aead.maxauthsize = alg->cra_aead.maxauthsize;

	inst->alg.cra_ctxsize = sizeof(struct pcrypt_aead_ctx);

	inst->alg.cra_init = pcrypt_aead_init_tfm;
	inst->alg.cra_exit = pcrypt_aead_exit_tfm;

	inst->alg.cra_aead.setkey = pcrypt_aead_setkey;
	inst->alg.cra_aead.setauthsize = pcrypt_aead_setauthsize;
	inst->alg.cra_aead.encrypt = pcrypt_aead_encrypt;
	inst->alg.cra_aead.decrypt = pcrypt_aead_decrypt;
	inst->alg.cra_aead.givencrypt = pcrypt_aead_givencrypt;

out_put_alg:
	crypto_mod_put(alg);
	return inst;
}
