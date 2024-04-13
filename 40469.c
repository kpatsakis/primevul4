static unsigned int crypto_blkcipher_ctxsize(struct crypto_alg *alg, u32 type,
					     u32 mask)
{
	struct blkcipher_alg *cipher = &alg->cra_blkcipher;
	unsigned int len = alg->cra_ctxsize;

	if ((mask & CRYPTO_ALG_TYPE_MASK) == CRYPTO_ALG_TYPE_MASK &&
	    cipher->ivsize) {
		len = ALIGN(len, (unsigned long)alg->cra_alignmask + 1);
		len += cipher->ivsize;
	}

	return len;
}
