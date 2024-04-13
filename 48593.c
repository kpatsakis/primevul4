static struct crypto_instance *seqiv_ablkcipher_alloc(struct rtattr **tb)
{
	struct crypto_instance *inst;

	inst = skcipher_geniv_alloc(&seqiv_tmpl, tb, 0, 0);

	if (IS_ERR(inst))
		goto out;

	inst->alg.cra_ablkcipher.givencrypt = seqiv_givencrypt_first;

	inst->alg.cra_init = seqiv_init;
	inst->alg.cra_exit = skcipher_geniv_exit;

	inst->alg.cra_ctxsize += inst->alg.cra_ablkcipher.ivsize;

out:
	return inst;
}
