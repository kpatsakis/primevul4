static void crypto_rng_show(struct seq_file *m, struct crypto_alg *alg)
{
	seq_printf(m, "type         : rng\n");
 	seq_printf(m, "seedsize     : %u\n", seedsize(alg));
 }
