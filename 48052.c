get_reserved_cluster_alloc(struct inode *inode, ext4_lblk_t lblk_start,
			   unsigned int num_blks)
{
	struct ext4_sb_info *sbi = EXT4_SB(inode->i_sb);
	ext4_lblk_t alloc_cluster_start, alloc_cluster_end;
	ext4_lblk_t lblk_from, lblk_to, c_offset;
	unsigned int allocated_clusters = 0;

	alloc_cluster_start = EXT4_B2C(sbi, lblk_start);
	alloc_cluster_end = EXT4_B2C(sbi, lblk_start + num_blks - 1);

	/* max possible clusters for this allocation */
	allocated_clusters = alloc_cluster_end - alloc_cluster_start + 1;

	trace_ext4_get_reserved_cluster_alloc(inode, lblk_start, num_blks);

	/* Check towards left side */
	c_offset = EXT4_LBLK_COFF(sbi, lblk_start);
	if (c_offset) {
		lblk_from = EXT4_LBLK_CMASK(sbi, lblk_start);
		lblk_to = lblk_from + c_offset - 1;

		if (ext4_find_delalloc_range(inode, lblk_from, lblk_to))
			allocated_clusters--;
	}

	/* Now check towards right. */
	c_offset = EXT4_LBLK_COFF(sbi, lblk_start + num_blks);
	if (allocated_clusters && c_offset) {
		lblk_from = lblk_start + num_blks;
		lblk_to = lblk_from + (sbi->s_cluster_ratio - c_offset) - 1;

		if (ext4_find_delalloc_range(inode, lblk_from, lblk_to))
			allocated_clusters--;
	}

	return allocated_clusters;
}
