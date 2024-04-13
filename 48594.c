static struct crypto_instance *seqiv_aead_alloc(struct rtattr **tb)
{
	struct crypto_instance *inst;

	inst = aead_geniv_alloc(&seqiv_tmpl, tb, 0, 0);

	if (IS_ERR(inst))
		goto out;

	inst->alg.cra_aead.givencrypt = seqiv_aead_givencrypt_first;

	inst->alg.cra_init = seqiv_aead_init;
	inst->alg.cra_exit = aead_geniv_exit;

	inst->alg.cra_ctxsize = inst->alg.cra_aead.ivsize;

out:
	return inst;
}
