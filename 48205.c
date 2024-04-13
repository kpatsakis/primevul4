static int parse_header_metadata(struct ecryptfs_crypt_stat *crypt_stat,
				 char *virt, int *bytes_read,
				 int validate_header_size)
{
	int rc = 0;
	u32 header_extent_size;
	u16 num_header_extents_at_front;

	header_extent_size = get_unaligned_be32(virt);
	virt += sizeof(__be32);
	num_header_extents_at_front = get_unaligned_be16(virt);
	crypt_stat->metadata_size = (((size_t)num_header_extents_at_front
				     * (size_t)header_extent_size));
	(*bytes_read) = (sizeof(__be32) + sizeof(__be16));
	if ((validate_header_size == ECRYPTFS_VALIDATE_HEADER_SIZE)
	    && (crypt_stat->metadata_size
		< ECRYPTFS_MINIMUM_HEADER_EXTENT_SIZE)) {
		rc = -EINVAL;
		printk(KERN_WARNING "Invalid header size: [%zd]\n",
		       crypt_stat->metadata_size);
	}
	return rc;
}
