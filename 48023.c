ext4_ext_put_gap_in_cache(struct inode *inode, struct ext4_ext_path *path,
				ext4_lblk_t block)
{
	int depth = ext_depth(inode);
	ext4_lblk_t len;
	ext4_lblk_t lblock;
	struct ext4_extent *ex;
	struct extent_status es;

	ex = path[depth].p_ext;
	if (ex == NULL) {
		/* there is no extent yet, so gap is [0;-] */
		lblock = 0;
		len = EXT_MAX_BLOCKS;
		ext_debug("cache gap(whole file):");
	} else if (block < le32_to_cpu(ex->ee_block)) {
		lblock = block;
		len = le32_to_cpu(ex->ee_block) - block;
		ext_debug("cache gap(before): %u [%u:%u]",
				block,
				le32_to_cpu(ex->ee_block),
				 ext4_ext_get_actual_len(ex));
	} else if (block >= le32_to_cpu(ex->ee_block)
			+ ext4_ext_get_actual_len(ex)) {
		ext4_lblk_t next;
		lblock = le32_to_cpu(ex->ee_block)
			+ ext4_ext_get_actual_len(ex);

		next = ext4_ext_next_allocated_block(path);
		ext_debug("cache gap(after): [%u:%u] %u",
				le32_to_cpu(ex->ee_block),
				ext4_ext_get_actual_len(ex),
				block);
		BUG_ON(next == lblock);
		len = next - lblock;
	} else {
		BUG();
	}

	ext4_es_find_delayed_extent_range(inode, lblock, lblock + len - 1, &es);
	if (es.es_len) {
		/* There's delayed extent containing lblock? */
		if (es.es_lblk <= lblock)
			return;
		len = min(es.es_lblk - lblock, len);
	}
	ext_debug(" -> %u:%u\n", lblock, len);
	ext4_es_insert_extent(inode, lblock, len, ~0, EXTENT_STATUS_HOLE);
}
