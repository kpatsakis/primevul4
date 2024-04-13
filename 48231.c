static void crypto_destroy_instance(struct crypto_alg *alg)
{
	struct crypto_instance *inst = (void *)alg;
	struct crypto_template *tmpl = inst->tmpl;

	tmpl->free(inst);
	crypto_tmpl_put(tmpl);
}
