static struct buffer_head * ext4_find_entry (struct inode *dir,
					const struct qstr *d_name,
					struct ext4_dir_entry_2 **res_dir,
					int *inlined)
{
	struct super_block *sb;
	struct buffer_head *bh_use[NAMEI_RA_SIZE];
	struct buffer_head *bh, *ret = NULL;
	ext4_lblk_t start, block, b;
	const u8 *name = d_name->name;
	int ra_max = 0;		/* Number of bh's in the readahead
				   buffer, bh_use[] */
	int ra_ptr = 0;		/* Current index into readahead
				   buffer */
	int num = 0;
	ext4_lblk_t  nblocks;
	int i, err;
	int namelen;

	*res_dir = NULL;
	sb = dir->i_sb;
	namelen = d_name->len;
	if (namelen > EXT4_NAME_LEN)
		return NULL;

	if (ext4_has_inline_data(dir)) {
		int has_inline_data = 1;
		ret = ext4_find_inline_entry(dir, d_name, res_dir,
					     &has_inline_data);
		if (has_inline_data) {
			if (inlined)
				*inlined = 1;
			return ret;
		}
	}

	if ((namelen <= 2) && (name[0] == '.') &&
	    (name[1] == '.' || name[1] == '\0')) {
		/*
		 * "." or ".." will only be in the first block
		 * NFS may look up ".."; "." should be handled by the VFS
		 */
		block = start = 0;
		nblocks = 1;
		goto restart;
	}
	if (is_dx(dir)) {
		bh = ext4_dx_find_entry(dir, d_name, res_dir, &err);
		/*
		 * On success, or if the error was file not found,
		 * return.  Otherwise, fall back to doing a search the
		 * old fashioned way.
		 */
		if (bh || (err != ERR_BAD_DX_DIR))
			return bh;
		dxtrace(printk(KERN_DEBUG "ext4_find_entry: dx failed, "
			       "falling back\n"));
	}
	nblocks = dir->i_size >> EXT4_BLOCK_SIZE_BITS(sb);
	start = EXT4_I(dir)->i_dir_start_lookup;
	if (start >= nblocks)
		start = 0;
	block = start;
restart:
	do {
		/*
		 * We deal with the read-ahead logic here.
		 */
		if (ra_ptr >= ra_max) {
			/* Refill the readahead buffer */
			ra_ptr = 0;
			b = block;
			for (ra_max = 0; ra_max < NAMEI_RA_SIZE; ra_max++) {
				/*
				 * Terminate if we reach the end of the
				 * directory and must wrap, or if our
				 * search has finished at this block.
				 */
				if (b >= nblocks || (num && block == start)) {
					bh_use[ra_max] = NULL;
					break;
				}
				num++;
				bh = ext4_getblk(NULL, dir, b++, 0, &err);
				bh_use[ra_max] = bh;
				if (bh)
					ll_rw_block(READ | REQ_META | REQ_PRIO,
						    1, &bh);
			}
		}
		if ((bh = bh_use[ra_ptr++]) == NULL)
			goto next;
		wait_on_buffer(bh);
		if (!buffer_uptodate(bh)) {
			/* read error, skip block & hope for the best */
			EXT4_ERROR_INODE(dir, "reading directory lblock %lu",
					 (unsigned long) block);
			brelse(bh);
			goto next;
		}
		if (!buffer_verified(bh) &&
		    !is_dx_internal_node(dir, block,
					 (struct ext4_dir_entry *)bh->b_data) &&
		    !ext4_dirent_csum_verify(dir,
				(struct ext4_dir_entry *)bh->b_data)) {
			EXT4_ERROR_INODE(dir, "checksumming directory "
					 "block %lu", (unsigned long)block);
			brelse(bh);
			goto next;
		}
		set_buffer_verified(bh);
		i = search_dirblock(bh, dir, d_name,
			    block << EXT4_BLOCK_SIZE_BITS(sb), res_dir);
		if (i == 1) {
			EXT4_I(dir)->i_dir_start_lookup = block;
			ret = bh;
			goto cleanup_and_exit;
		} else {
			brelse(bh);
			if (i < 0)
				goto cleanup_and_exit;
		}
	next:
		if (++block >= nblocks)
			block = 0;
	} while (block != start);

	/*
	 * If the directory has grown while we were searching, then
	 * search the last part of the directory before giving up.
	 */
	block = nblocks;
	nblocks = dir->i_size >> EXT4_BLOCK_SIZE_BITS(sb);
	if (block < nblocks) {
		start = 0;
		goto restart;
	}

cleanup_and_exit:
	/* Clean up the read-ahead blocks */
	for (; ra_ptr < ra_max; ra_ptr++)
		brelse(bh_use[ra_ptr]);
	return ret;
}
