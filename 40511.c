static void crypto_exit_shash_ops_compat(struct crypto_tfm *tfm)
{
	struct shash_desc **descp = crypto_tfm_ctx(tfm);
	struct shash_desc *desc = *descp;

	crypto_free_shash(desc->tfm);
	kzfree(desc);
}
