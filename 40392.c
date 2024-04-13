const char *crypto_default_geniv(const struct crypto_alg *alg)
{
	if (((alg->cra_flags & CRYPTO_ALG_TYPE_MASK) ==
	     CRYPTO_ALG_TYPE_BLKCIPHER ? alg->cra_blkcipher.ivsize :
					 alg->cra_ablkcipher.ivsize) !=
	    alg->cra_blocksize)
		return "chainiv";

	return alg->cra_flags & CRYPTO_ALG_ASYNC ?
	       "eseqiv" : skcipher_default_geniv;
}
