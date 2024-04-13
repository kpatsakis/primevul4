static struct crypto_blkcipher *ceph_crypto_alloc_cipher(void)
{
	return crypto_alloc_blkcipher("cbc(aes)", 0, CRYPTO_ALG_ASYNC);
}
