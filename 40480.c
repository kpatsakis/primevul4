int skcipher_geniv_init(struct crypto_tfm *tfm)
{
	struct crypto_instance *inst = (void *)tfm->__crt_alg;
	struct crypto_ablkcipher *cipher;

	cipher = crypto_spawn_skcipher(crypto_instance_ctx(inst));
	if (IS_ERR(cipher))
		return PTR_ERR(cipher);

	tfm->crt_ablkcipher.base = cipher;
	tfm->crt_ablkcipher.reqsize += crypto_ablkcipher_reqsize(cipher);

	return 0;
}
