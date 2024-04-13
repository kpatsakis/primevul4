ecryptfs_init_crypt_stat(struct ecryptfs_crypt_stat *crypt_stat)
{
	memset((void *)crypt_stat, 0, sizeof(struct ecryptfs_crypt_stat));
	INIT_LIST_HEAD(&crypt_stat->keysig_list);
	mutex_init(&crypt_stat->keysig_list_mutex);
	mutex_init(&crypt_stat->cs_mutex);
	mutex_init(&crypt_stat->cs_tfm_mutex);
	mutex_init(&crypt_stat->cs_hash_tfm_mutex);
	crypt_stat->flags |= ECRYPTFS_STRUCT_INITIALIZED;
}
