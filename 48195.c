int ecryptfs_set_f_namelen(long *namelen, long lower_namelen,
			   struct ecryptfs_mount_crypt_stat *mount_crypt_stat)
{
	struct blkcipher_desc desc;
	struct mutex *tfm_mutex;
	size_t cipher_blocksize;
	int rc;

	if (!(mount_crypt_stat->flags & ECRYPTFS_GLOBAL_ENCRYPT_FILENAMES)) {
		(*namelen) = lower_namelen;
		return 0;
	}

	rc = ecryptfs_get_tfm_and_mutex_for_cipher_name(&desc.tfm, &tfm_mutex,
			mount_crypt_stat->global_default_fn_cipher_name);
	if (unlikely(rc)) {
		(*namelen) = 0;
		return rc;
	}

	mutex_lock(tfm_mutex);
	cipher_blocksize = crypto_blkcipher_blocksize(desc.tfm);
	mutex_unlock(tfm_mutex);

	/* Return an exact amount for the common cases */
	if (lower_namelen == NAME_MAX
	    && (cipher_blocksize == 8 || cipher_blocksize == 16)) {
		(*namelen) = ENC_NAME_MAX_BLOCKLEN_8_OR_16;
		return 0;
	}

	/* Return a safe estimate for the uncommon cases */
	(*namelen) = lower_namelen;
	(*namelen) -= ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX_SIZE;
	/* Since this is the max decoded size, subtract 1 "decoded block" len */
	(*namelen) = ecryptfs_max_decoded_size(*namelen) - 3;
	(*namelen) -= ECRYPTFS_TAG_70_MAX_METADATA_SIZE;
	(*namelen) -= ECRYPTFS_FILENAME_MIN_RANDOM_PREPEND_BYTES;
	/* Worst case is that the filename is padded nearly a full block size */
	(*namelen) -= cipher_blocksize - 1;

	if ((*namelen) < 0)
		(*namelen) = 0;

	return 0;
}
