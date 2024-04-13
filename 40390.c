static void crypto_ablkcipher_show(struct seq_file *m, struct crypto_alg *alg)
{
	struct ablkcipher_alg *ablkcipher = &alg->cra_ablkcipher;

	seq_printf(m, "type         : ablkcipher\n");
	seq_printf(m, "async        : %s\n", alg->cra_flags & CRYPTO_ALG_ASYNC ?
					     "yes" : "no");
	seq_printf(m, "blocksize    : %u\n", alg->cra_blocksize);
	seq_printf(m, "min keysize  : %u\n", ablkcipher->min_keysize);
	seq_printf(m, "max keysize  : %u\n", ablkcipher->max_keysize);
	seq_printf(m, "ivsize       : %u\n", ablkcipher->ivsize);
	seq_printf(m, "geniv        : %s\n", ablkcipher->geniv ?: "<default>");
}
