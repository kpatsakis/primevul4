int ecryptfs_read_and_validate_header_region(struct inode *inode)
{
	u8 file_size[ECRYPTFS_SIZE_AND_MARKER_BYTES];
	u8 *marker = file_size + ECRYPTFS_FILE_SIZE_BYTES;
	int rc;

	rc = ecryptfs_read_lower(file_size, 0, ECRYPTFS_SIZE_AND_MARKER_BYTES,
				 inode);
	if (rc < ECRYPTFS_SIZE_AND_MARKER_BYTES)
		return rc >= 0 ? -EINVAL : rc;
	rc = ecryptfs_validate_marker(marker);
	if (!rc)
		ecryptfs_i_size_init(file_size, inode);
	return rc;
}
