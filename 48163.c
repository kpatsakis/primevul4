ecryptfs_add_new_key_tfm(struct ecryptfs_key_tfm **key_tfm, char *cipher_name,
			 size_t key_size)
{
	struct ecryptfs_key_tfm *tmp_tfm;
	int rc = 0;

	BUG_ON(!mutex_is_locked(&key_tfm_list_mutex));

	tmp_tfm = kmem_cache_alloc(ecryptfs_key_tfm_cache, GFP_KERNEL);
	if (key_tfm != NULL)
		(*key_tfm) = tmp_tfm;
	if (!tmp_tfm) {
		rc = -ENOMEM;
		printk(KERN_ERR "Error attempting to allocate from "
		       "ecryptfs_key_tfm_cache\n");
		goto out;
	}
	mutex_init(&tmp_tfm->key_tfm_mutex);
	strncpy(tmp_tfm->cipher_name, cipher_name,
		ECRYPTFS_MAX_CIPHER_NAME_SIZE);
	tmp_tfm->cipher_name[ECRYPTFS_MAX_CIPHER_NAME_SIZE] = '\0';
	tmp_tfm->key_size = key_size;
	rc = ecryptfs_process_key_cipher(&tmp_tfm->key_tfm,
					 tmp_tfm->cipher_name,
					 &tmp_tfm->key_size);
	if (rc) {
		printk(KERN_ERR "Error attempting to initialize key TFM "
		       "cipher with name = [%s]; rc = [%d]\n",
		       tmp_tfm->cipher_name, rc);
		kmem_cache_free(ecryptfs_key_tfm_cache, tmp_tfm);
		if (key_tfm != NULL)
			(*key_tfm) = NULL;
		goto out;
	}
	list_add(&tmp_tfm->key_tfm_list, &key_tfm_list);
out:
	return rc;
}
