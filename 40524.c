static void crypto_shash_show(struct seq_file *m, struct crypto_alg *alg)
{
	struct shash_alg *salg = __crypto_shash_alg(alg);

	seq_printf(m, "type         : shash\n");
	seq_printf(m, "blocksize    : %u\n", alg->cra_blocksize);
	seq_printf(m, "digestsize   : %u\n", salg->digestsize);
}
