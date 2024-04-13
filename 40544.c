int shash_register_instance(struct crypto_template *tmpl,
			    struct shash_instance *inst)
{
	int err;

	err = shash_prepare_alg(&inst->alg);
	if (err)
		return err;

	return crypto_register_instance(tmpl, shash_crypto_instance(inst));
}
