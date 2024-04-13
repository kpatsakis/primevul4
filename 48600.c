static struct crypto_instance *seqiv_alloc(struct rtattr **tb)
{
	struct crypto_attr_type *algt;
	struct crypto_instance *inst;
	int err;

	algt = crypto_get_attr_type(tb);
	if (IS_ERR(algt))
		return ERR_CAST(algt);

	err = crypto_get_default_rng();
	if (err)
		return ERR_PTR(err);

	if ((algt->type ^ CRYPTO_ALG_TYPE_AEAD) & CRYPTO_ALG_TYPE_MASK)
		inst = seqiv_ablkcipher_alloc(tb);
	else
		inst = seqiv_aead_alloc(tb);

	if (IS_ERR(inst))
		goto put_rng;

	inst->alg.cra_alignmask |= __alignof__(u32) - 1;
	inst->alg.cra_ctxsize += sizeof(struct seqiv_ctx);

out:
	return inst;

put_rng:
	crypto_put_default_rng();
	goto out;
}
