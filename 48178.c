ecryptfs_encrypt_filename(struct ecryptfs_filename *filename,
			  struct ecryptfs_crypt_stat *crypt_stat,
			  struct ecryptfs_mount_crypt_stat *mount_crypt_stat)
{
	int rc = 0;

	filename->encrypted_filename = NULL;
	filename->encrypted_filename_size = 0;
	if ((crypt_stat && (crypt_stat->flags & ECRYPTFS_ENCFN_USE_MOUNT_FNEK))
	    || (mount_crypt_stat && (mount_crypt_stat->flags
				     & ECRYPTFS_GLOBAL_ENCFN_USE_MOUNT_FNEK))) {
		size_t packet_size;
		size_t remaining_bytes;

		rc = ecryptfs_write_tag_70_packet(
			NULL, NULL,
			&filename->encrypted_filename_size,
			mount_crypt_stat, NULL,
			filename->filename_size);
		if (rc) {
			printk(KERN_ERR "%s: Error attempting to get packet "
			       "size for tag 72; rc = [%d]\n", __func__,
			       rc);
			filename->encrypted_filename_size = 0;
			goto out;
		}
		filename->encrypted_filename =
			kmalloc(filename->encrypted_filename_size, GFP_KERNEL);
		if (!filename->encrypted_filename) {
			printk(KERN_ERR "%s: Out of memory whilst attempting "
			       "to kmalloc [%zd] bytes\n", __func__,
			       filename->encrypted_filename_size);
			rc = -ENOMEM;
			goto out;
		}
		remaining_bytes = filename->encrypted_filename_size;
		rc = ecryptfs_write_tag_70_packet(filename->encrypted_filename,
						  &remaining_bytes,
						  &packet_size,
						  mount_crypt_stat,
						  filename->filename,
						  filename->filename_size);
		if (rc) {
			printk(KERN_ERR "%s: Error attempting to generate "
			       "tag 70 packet; rc = [%d]\n", __func__,
			       rc);
			kfree(filename->encrypted_filename);
			filename->encrypted_filename = NULL;
			filename->encrypted_filename_size = 0;
			goto out;
		}
		filename->encrypted_filename_size = packet_size;
	} else {
		printk(KERN_ERR "%s: No support for requested filename "
		       "encryption method in this release\n", __func__);
		rc = -EOPNOTSUPP;
		goto out;
	}
out:
	return rc;
}
