ecryptfs_process_key_cipher(struct crypto_blkcipher **key_tfm,
			    char *cipher_name, size_t *key_size)
{
	char dummy_key[ECRYPTFS_MAX_KEY_BYTES];
	char *full_alg_name = NULL;
	int rc;

	*key_tfm = NULL;
	if (*key_size > ECRYPTFS_MAX_KEY_BYTES) {
		rc = -EINVAL;
		printk(KERN_ERR "Requested key size is [%zd] bytes; maximum "
		      "allowable is [%d]\n", *key_size, ECRYPTFS_MAX_KEY_BYTES);
		goto out;
	}
	rc = ecryptfs_crypto_api_algify_cipher_name(&full_alg_name, cipher_name,
						    "ecb");
	if (rc)
		goto out;
	*key_tfm = crypto_alloc_blkcipher(full_alg_name, 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(*key_tfm)) {
		rc = PTR_ERR(*key_tfm);
		printk(KERN_ERR "Unable to allocate crypto cipher with name "
		       "[%s]; rc = [%d]\n", full_alg_name, rc);
		goto out;
	}
	crypto_blkcipher_set_flags(*key_tfm, CRYPTO_TFM_REQ_WEAK_KEY);
	if (*key_size == 0) {
		struct blkcipher_alg *alg = crypto_blkcipher_alg(*key_tfm);

		*key_size = alg->max_keysize;
	}
	get_random_bytes(dummy_key, *key_size);
	rc = crypto_blkcipher_setkey(*key_tfm, dummy_key, *key_size);
	if (rc) {
		printk(KERN_ERR "Error attempting to set key of size [%zd] for "
		       "cipher [%s]; rc = [%d]\n", *key_size, full_alg_name,
		       rc);
		rc = -EINVAL;
		goto out;
	}
out:
	kfree(full_alg_name);
	return rc;
}
