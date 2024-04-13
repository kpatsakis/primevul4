static int __ext4_ext_check(const char *function, unsigned int line,
			    struct inode *inode, struct ext4_extent_header *eh,
			    int depth, ext4_fsblk_t pblk)
{
	const char *error_msg;
	int max = 0;

	if (unlikely(eh->eh_magic != EXT4_EXT_MAGIC)) {
		error_msg = "invalid magic";
		goto corrupted;
	}
	if (unlikely(le16_to_cpu(eh->eh_depth) != depth)) {
		error_msg = "unexpected eh_depth";
		goto corrupted;
	}
	if (unlikely(eh->eh_max == 0)) {
		error_msg = "invalid eh_max";
		goto corrupted;
	}
	max = ext4_ext_max_entries(inode, depth);
	if (unlikely(le16_to_cpu(eh->eh_max) > max)) {
		error_msg = "too large eh_max";
		goto corrupted;
	}
	if (unlikely(le16_to_cpu(eh->eh_entries) > le16_to_cpu(eh->eh_max))) {
		error_msg = "invalid eh_entries";
		goto corrupted;
	}
	if (!ext4_valid_extent_entries(inode, eh, depth)) {
		error_msg = "invalid extent entries";
		goto corrupted;
	}
	/* Verify checksum on non-root extent tree nodes */
	if (ext_depth(inode) != depth &&
	    !ext4_extent_block_csum_verify(inode, eh)) {
		error_msg = "extent tree corrupted";
		goto corrupted;
	}
	return 0;

corrupted:
	ext4_error_inode(inode, function, line, 0,
			 "pblk %llu bad header/extent: %s - magic %x, "
			 "entries %u, max %u(%u), depth %u(%u)",
			 (unsigned long long) pblk, error_msg,
			 le16_to_cpu(eh->eh_magic),
			 le16_to_cpu(eh->eh_entries), le16_to_cpu(eh->eh_max),
			 max, le16_to_cpu(eh->eh_depth), depth);
	return -EIO;
}
