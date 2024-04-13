int ecryptfs_decode_and_decrypt_filename(char **plaintext_name,
					 size_t *plaintext_name_size,
					 struct super_block *sb,
					 const char *name, size_t name_size)
{
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat =
		&ecryptfs_superblock_to_private(sb)->mount_crypt_stat;
	char *decoded_name;
	size_t decoded_name_size;
	size_t packet_size;
	int rc = 0;

	if ((mount_crypt_stat->flags & ECRYPTFS_GLOBAL_ENCRYPT_FILENAMES)
	    && !(mount_crypt_stat->flags & ECRYPTFS_ENCRYPTED_VIEW_ENABLED)
	    && (name_size > ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX_SIZE)
	    && (strncmp(name, ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX,
			ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX_SIZE) == 0)) {
		const char *orig_name = name;
		size_t orig_name_size = name_size;

		name += ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX_SIZE;
		name_size -= ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX_SIZE;
		ecryptfs_decode_from_filename(NULL, &decoded_name_size,
					      name, name_size);
		decoded_name = kmalloc(decoded_name_size, GFP_KERNEL);
		if (!decoded_name) {
			printk(KERN_ERR "%s: Out of memory whilst attempting "
			       "to kmalloc [%zd] bytes\n", __func__,
			       decoded_name_size);
			rc = -ENOMEM;
			goto out;
		}
		ecryptfs_decode_from_filename(decoded_name, &decoded_name_size,
					      name, name_size);
		rc = ecryptfs_parse_tag_70_packet(plaintext_name,
						  plaintext_name_size,
						  &packet_size,
						  mount_crypt_stat,
						  decoded_name,
						  decoded_name_size);
		if (rc) {
			printk(KERN_INFO "%s: Could not parse tag 70 packet "
			       "from filename; copying through filename "
			       "as-is\n", __func__);
			rc = ecryptfs_copy_filename(plaintext_name,
						    plaintext_name_size,
						    orig_name, orig_name_size);
			goto out_free;
		}
	} else {
		rc = ecryptfs_copy_filename(plaintext_name,
					    plaintext_name_size,
					    name, name_size);
		goto out;
	}
out_free:
	kfree(decoded_name);
out:
	return rc;
}
