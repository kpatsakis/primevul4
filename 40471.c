static void crypto_blkcipher_show(struct seq_file *m, struct crypto_alg *alg)
{
	seq_printf(m, "type         : blkcipher\n");
	seq_printf(m, "blocksize    : %u\n", alg->cra_blocksize);
	seq_printf(m, "min keysize  : %u\n", alg->cra_blkcipher.min_keysize);
	seq_printf(m, "max keysize  : %u\n", alg->cra_blkcipher.max_keysize);
	seq_printf(m, "ivsize       : %u\n", alg->cra_blkcipher.ivsize);
	seq_printf(m, "geniv        : %s\n", alg->cra_blkcipher.geniv ?:
					     "<default>");
}
