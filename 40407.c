int aead_geniv_init(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct crypto_aead *aead;

	aead = crypto_spawn_aead(crypto_instance_ctx(inst));
	if (IS_ERR(aead))
		return PTR_ERR(aead);

	tfm->crt_aead.base = aead;
	tfm->crt_aead.reqsize += crypto_aead_reqsize(aead);

	return 0;
}
