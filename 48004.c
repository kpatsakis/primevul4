__read_extent_tree_block(const char *function, unsigned int line,
			 struct inode *inode, ext4_fsblk_t pblk, int depth,
			 int flags)
{
	struct buffer_head		*bh;
	int				err;

	bh = sb_getblk(inode->i_sb, pblk);
	if (unlikely(!bh))
		return ERR_PTR(-ENOMEM);

	if (!bh_uptodate_or_lock(bh)) {
		trace_ext4_ext_load_extent(inode, pblk, _RET_IP_);
		err = bh_submit_read(bh);
		if (err < 0)
			goto errout;
	}
	if (buffer_verified(bh) && !(flags & EXT4_EX_FORCE_CACHE))
		return bh;
	err = __ext4_ext_check(function, line, inode,
			       ext_block_hdr(bh), depth, pblk);
	if (err)
		goto errout;
	set_buffer_verified(bh);
	/*
	 * If this is a leaf block, cache all of its entries
	 */
	if (!(flags & EXT4_EX_NOCACHE) && depth == 0) {
		struct ext4_extent_header *eh = ext_block_hdr(bh);
		struct ext4_extent *ex = EXT_FIRST_EXTENT(eh);
		ext4_lblk_t prev = 0;
		int i;

		for (i = le16_to_cpu(eh->eh_entries); i > 0; i--, ex++) {
			unsigned int status = EXTENT_STATUS_WRITTEN;
			ext4_lblk_t lblk = le32_to_cpu(ex->ee_block);
			int len = ext4_ext_get_actual_len(ex);

			if (prev && (prev != lblk))
				ext4_es_cache_extent(inode, prev,
						     lblk - prev, ~0,
						     EXTENT_STATUS_HOLE);

			if (ext4_ext_is_unwritten(ex))
				status = EXTENT_STATUS_UNWRITTEN;
			ext4_es_cache_extent(inode, lblk, len,
					     ext4_ext_pblock(ex), status);
			prev = lblk + len;
		}
	}
	return bh;
errout:
	put_bh(bh);
	return ERR_PTR(err);

}
