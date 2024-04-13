int skcipher_register_instance(struct crypto_template *tmpl,
			   struct skcipher_instance *inst)
{
	int err;

	err = skcipher_prepare_alg(&inst->alg);
	if (err)
		return err;

	return crypto_register_instance(tmpl, skcipher_crypto_instance(inst));
}
