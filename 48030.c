static void ext4_ext_show_path(struct inode *inode, struct ext4_ext_path *path)
{
	int k, l = path->p_depth;

	ext_debug("path:");
	for (k = 0; k <= l; k++, path++) {
		if (path->p_idx) {
		  ext_debug("  %d->%llu", le32_to_cpu(path->p_idx->ei_block),
			    ext4_idx_pblock(path->p_idx));
		} else if (path->p_ext) {
			ext_debug("  %d:[%d]%d:%llu ",
				  le32_to_cpu(path->p_ext->ee_block),
				  ext4_ext_is_unwritten(path->p_ext),
				  ext4_ext_get_actual_len(path->p_ext),
				  ext4_ext_pblock(path->p_ext));
		} else
			ext_debug("  []");
	}
	ext_debug("\n");
}
