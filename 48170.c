static int ecryptfs_copy_mount_wide_sigs_to_inode_sigs(
	struct ecryptfs_crypt_stat *crypt_stat,
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat)
{
	struct ecryptfs_global_auth_tok *global_auth_tok;
	int rc = 0;

	mutex_lock(&crypt_stat->keysig_list_mutex);
	mutex_lock(&mount_crypt_stat->global_auth_tok_list_mutex);

	list_for_each_entry(global_auth_tok,
			    &mount_crypt_stat->global_auth_tok_list,
			    mount_crypt_stat_list) {
		if (global_auth_tok->flags & ECRYPTFS_AUTH_TOK_FNEK)
			continue;
		rc = ecryptfs_add_keysig(crypt_stat, global_auth_tok->sig);
		if (rc) {
			printk(KERN_ERR "Error adding keysig; rc = [%d]\n", rc);
			goto out;
		}
	}

out:
	mutex_unlock(&mount_crypt_stat->global_auth_tok_list_mutex);
	mutex_unlock(&crypt_stat->keysig_list_mutex);
	return rc;
}
