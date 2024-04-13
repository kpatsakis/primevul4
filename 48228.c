static int crypto_check_alg(struct crypto_alg *alg)
{
	crypto_check_module_sig(alg->cra_module);

	if (alg->cra_alignmask & (alg->cra_alignmask + 1))
		return -EINVAL;

	if (alg->cra_blocksize > PAGE_SIZE / 8)
		return -EINVAL;

	if (alg->cra_priority < 0)
		return -EINVAL;

	return crypto_set_driver_name(alg);
}
