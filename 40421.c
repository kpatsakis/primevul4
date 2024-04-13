static void crypto_nivaead_show(struct seq_file *m, struct crypto_alg *alg)
{
	struct aead_alg *aead = &alg->cra_aead;

	seq_printf(m, "type         : nivaead\n");
	seq_printf(m, "async        : %s\n", alg->cra_flags & CRYPTO_ALG_ASYNC ?
					     "yes" : "no");
	seq_printf(m, "blocksize    : %u\n", alg->cra_blocksize);
	seq_printf(m, "ivsize       : %u\n", aead->ivsize);
	seq_printf(m, "maxauthsize  : %u\n", aead->maxauthsize);
	seq_printf(m, "geniv        : %s\n", aead->geniv);
}
