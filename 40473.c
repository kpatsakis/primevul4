static int crypto_init_blkcipher_ops(struct crypto_tfm *tfm, u32 type, u32 mask)
{
	struct blkcipher_alg *alg = &tfm->__crt_alg->cra_blkcipher;

	if (alg->ivsize > PAGE_SIZE / 8)
		return -EINVAL;

	if ((mask & CRYPTO_ALG_TYPE_MASK) == CRYPTO_ALG_TYPE_MASK)
		return crypto_init_blkcipher_ops_sync(tfm);
	else
		return crypto_init_blkcipher_ops_async(tfm);
}
