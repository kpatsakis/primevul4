static int ecryptfs_calculate_md5(char *dst,
				  struct ecryptfs_crypt_stat *crypt_stat,
				  char *src, int len)
{
	struct scatterlist sg;
	struct hash_desc desc = {
		.tfm = crypt_stat->hash_tfm,
		.flags = CRYPTO_TFM_REQ_MAY_SLEEP
	};
	int rc = 0;

	mutex_lock(&crypt_stat->cs_hash_tfm_mutex);
	sg_init_one(&sg, (u8 *)src, len);
	if (!desc.tfm) {
		desc.tfm = crypto_alloc_hash(ECRYPTFS_DEFAULT_HASH, 0,
					     CRYPTO_ALG_ASYNC);
		if (IS_ERR(desc.tfm)) {
			rc = PTR_ERR(desc.tfm);
			ecryptfs_printk(KERN_ERR, "Error attempting to "
					"allocate crypto context; rc = [%d]\n",
					rc);
			goto out;
		}
		crypt_stat->hash_tfm = desc.tfm;
	}
	rc = crypto_hash_init(&desc);
	if (rc) {
		printk(KERN_ERR
		       "%s: Error initializing crypto hash; rc = [%d]\n",
		       __func__, rc);
		goto out;
	}
	rc = crypto_hash_update(&desc, &sg, len);
	if (rc) {
		printk(KERN_ERR
		       "%s: Error updating crypto hash; rc = [%d]\n",
		       __func__, rc);
		goto out;
	}
	rc = crypto_hash_final(&desc, dst);
	if (rc) {
		printk(KERN_ERR
		       "%s: Error finalizing crypto hash; rc = [%d]\n",
		       __func__, rc);
		goto out;
	}
out:
	mutex_unlock(&crypt_stat->cs_hash_tfm_mutex);
	return rc;
}
