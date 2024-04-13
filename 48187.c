static int ecryptfs_process_flags(struct ecryptfs_crypt_stat *crypt_stat,
				  char *page_virt, int *bytes_read)
{
	int rc = 0;
	int i;
	u32 flags;

	flags = get_unaligned_be32(page_virt);
	for (i = 0; i < ((sizeof(ecryptfs_flag_map)
			  / sizeof(struct ecryptfs_flag_map_elem))); i++)
		if (flags & ecryptfs_flag_map[i].file_flag) {
			crypt_stat->flags |= ecryptfs_flag_map[i].local_flag;
		} else
			crypt_stat->flags &= ~(ecryptfs_flag_map[i].local_flag);
	/* Version is in top 8 bits of the 32-bit flag vector */
	crypt_stat->file_version = ((flags >> 24) & 0xFF);
	(*bytes_read) = 4;
	return rc;
}
