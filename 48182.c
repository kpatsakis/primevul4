void ecryptfs_i_size_init(const char *page_virt, struct inode *inode)
{
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat;
	struct ecryptfs_crypt_stat *crypt_stat;
	u64 file_size;

	crypt_stat = &ecryptfs_inode_to_private(inode)->crypt_stat;
	mount_crypt_stat =
		&ecryptfs_superblock_to_private(inode->i_sb)->mount_crypt_stat;
	if (mount_crypt_stat->flags & ECRYPTFS_ENCRYPTED_VIEW_ENABLED) {
		file_size = i_size_read(ecryptfs_inode_to_lower(inode));
		if (crypt_stat->flags & ECRYPTFS_METADATA_IN_XATTR)
			file_size += crypt_stat->metadata_size;
	} else
		file_size = get_unaligned_be64(page_virt);
	i_size_write(inode, (loff_t)file_size);
	crypt_stat->flags |= ECRYPTFS_I_SIZE_INITIALIZED;
}
