static int aes_get_sizes(void)
{
	struct crypto_blkcipher *tfm;

	tfm = crypto_alloc_blkcipher(blkcipher_alg, 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(tfm)) {
		pr_err("encrypted_key: failed to alloc_cipher (%ld)\n",
		       PTR_ERR(tfm));
		return PTR_ERR(tfm);
	}
	ivsize = crypto_blkcipher_ivsize(tfm);
	blksize = crypto_blkcipher_blocksize(tfm);
	crypto_free_blkcipher(tfm);
	return 0;
}
