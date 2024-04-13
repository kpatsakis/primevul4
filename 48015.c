static int ext4_ext_convert_to_initialized(handle_t *handle,
					   struct inode *inode,
					   struct ext4_map_blocks *map,
					   struct ext4_ext_path **ppath,
					   int flags)
{
	struct ext4_ext_path *path = *ppath;
	struct ext4_sb_info *sbi;
	struct ext4_extent_header *eh;
	struct ext4_map_blocks split_map;
	struct ext4_extent zero_ex;
	struct ext4_extent *ex, *abut_ex;
	ext4_lblk_t ee_block, eof_block;
	unsigned int ee_len, depth, map_len = map->m_len;
	int allocated = 0, max_zeroout = 0;
	int err = 0;
	int split_flag = 0;

	ext_debug("ext4_ext_convert_to_initialized: inode %lu, logical"
		"block %llu, max_blocks %u\n", inode->i_ino,
		(unsigned long long)map->m_lblk, map_len);

	sbi = EXT4_SB(inode->i_sb);
	eof_block = (inode->i_size + inode->i_sb->s_blocksize - 1) >>
		inode->i_sb->s_blocksize_bits;
	if (eof_block < map->m_lblk + map_len)
		eof_block = map->m_lblk + map_len;

	depth = ext_depth(inode);
	eh = path[depth].p_hdr;
	ex = path[depth].p_ext;
	ee_block = le32_to_cpu(ex->ee_block);
	ee_len = ext4_ext_get_actual_len(ex);
	zero_ex.ee_len = 0;

	trace_ext4_ext_convert_to_initialized_enter(inode, map, ex);

	/* Pre-conditions */
	BUG_ON(!ext4_ext_is_unwritten(ex));
	BUG_ON(!in_range(map->m_lblk, ee_block, ee_len));

	/*
	 * Attempt to transfer newly initialized blocks from the currently
	 * unwritten extent to its neighbor. This is much cheaper
	 * than an insertion followed by a merge as those involve costly
	 * memmove() calls. Transferring to the left is the common case in
	 * steady state for workloads doing fallocate(FALLOC_FL_KEEP_SIZE)
	 * followed by append writes.
	 *
	 * Limitations of the current logic:
	 *  - L1: we do not deal with writes covering the whole extent.
	 *    This would require removing the extent if the transfer
	 *    is possible.
	 *  - L2: we only attempt to merge with an extent stored in the
	 *    same extent tree node.
	 */
	if ((map->m_lblk == ee_block) &&
		/* See if we can merge left */
		(map_len < ee_len) &&		/*L1*/
		(ex > EXT_FIRST_EXTENT(eh))) {	/*L2*/
		ext4_lblk_t prev_lblk;
		ext4_fsblk_t prev_pblk, ee_pblk;
		unsigned int prev_len;

		abut_ex = ex - 1;
		prev_lblk = le32_to_cpu(abut_ex->ee_block);
		prev_len = ext4_ext_get_actual_len(abut_ex);
		prev_pblk = ext4_ext_pblock(abut_ex);
		ee_pblk = ext4_ext_pblock(ex);

		/*
		 * A transfer of blocks from 'ex' to 'abut_ex' is allowed
		 * upon those conditions:
		 * - C1: abut_ex is initialized,
		 * - C2: abut_ex is logically abutting ex,
		 * - C3: abut_ex is physically abutting ex,
		 * - C4: abut_ex can receive the additional blocks without
		 *   overflowing the (initialized) length limit.
		 */
		if ((!ext4_ext_is_unwritten(abut_ex)) &&		/*C1*/
			((prev_lblk + prev_len) == ee_block) &&		/*C2*/
			((prev_pblk + prev_len) == ee_pblk) &&		/*C3*/
			(prev_len < (EXT_INIT_MAX_LEN - map_len))) {	/*C4*/
			err = ext4_ext_get_access(handle, inode, path + depth);
			if (err)
				goto out;

			trace_ext4_ext_convert_to_initialized_fastpath(inode,
				map, ex, abut_ex);

			/* Shift the start of ex by 'map_len' blocks */
			ex->ee_block = cpu_to_le32(ee_block + map_len);
			ext4_ext_store_pblock(ex, ee_pblk + map_len);
			ex->ee_len = cpu_to_le16(ee_len - map_len);
			ext4_ext_mark_unwritten(ex); /* Restore the flag */

			/* Extend abut_ex by 'map_len' blocks */
			abut_ex->ee_len = cpu_to_le16(prev_len + map_len);

			/* Result: number of initialized blocks past m_lblk */
			allocated = map_len;
		}
	} else if (((map->m_lblk + map_len) == (ee_block + ee_len)) &&
		   (map_len < ee_len) &&	/*L1*/
		   ex < EXT_LAST_EXTENT(eh)) {	/*L2*/
		/* See if we can merge right */
		ext4_lblk_t next_lblk;
		ext4_fsblk_t next_pblk, ee_pblk;
		unsigned int next_len;

		abut_ex = ex + 1;
		next_lblk = le32_to_cpu(abut_ex->ee_block);
		next_len = ext4_ext_get_actual_len(abut_ex);
		next_pblk = ext4_ext_pblock(abut_ex);
		ee_pblk = ext4_ext_pblock(ex);

		/*
		 * A transfer of blocks from 'ex' to 'abut_ex' is allowed
		 * upon those conditions:
		 * - C1: abut_ex is initialized,
		 * - C2: abut_ex is logically abutting ex,
		 * - C3: abut_ex is physically abutting ex,
		 * - C4: abut_ex can receive the additional blocks without
		 *   overflowing the (initialized) length limit.
		 */
		if ((!ext4_ext_is_unwritten(abut_ex)) &&		/*C1*/
		    ((map->m_lblk + map_len) == next_lblk) &&		/*C2*/
		    ((ee_pblk + ee_len) == next_pblk) &&		/*C3*/
		    (next_len < (EXT_INIT_MAX_LEN - map_len))) {	/*C4*/
			err = ext4_ext_get_access(handle, inode, path + depth);
			if (err)
				goto out;

			trace_ext4_ext_convert_to_initialized_fastpath(inode,
				map, ex, abut_ex);

			/* Shift the start of abut_ex by 'map_len' blocks */
			abut_ex->ee_block = cpu_to_le32(next_lblk - map_len);
			ext4_ext_store_pblock(abut_ex, next_pblk - map_len);
			ex->ee_len = cpu_to_le16(ee_len - map_len);
			ext4_ext_mark_unwritten(ex); /* Restore the flag */

			/* Extend abut_ex by 'map_len' blocks */
			abut_ex->ee_len = cpu_to_le16(next_len + map_len);

			/* Result: number of initialized blocks past m_lblk */
			allocated = map_len;
		}
	}
	if (allocated) {
		/* Mark the block containing both extents as dirty */
		ext4_ext_dirty(handle, inode, path + depth);

		/* Update path to point to the right extent */
		path[depth].p_ext = abut_ex;
		goto out;
	} else
		allocated = ee_len - (map->m_lblk - ee_block);

	WARN_ON(map->m_lblk < ee_block);
	/*
	 * It is safe to convert extent to initialized via explicit
	 * zeroout only if extent is fully inside i_size or new_size.
	 */
	split_flag |= ee_block + ee_len <= eof_block ? EXT4_EXT_MAY_ZEROOUT : 0;

	if (EXT4_EXT_MAY_ZEROOUT & split_flag)
		max_zeroout = sbi->s_extent_max_zeroout_kb >>
			(inode->i_sb->s_blocksize_bits - 10);

	/* If extent is less than s_max_zeroout_kb, zeroout directly */
	if (max_zeroout && (ee_len <= max_zeroout)) {
		err = ext4_ext_zeroout(inode, ex);
		if (err)
			goto out;
		zero_ex.ee_block = ex->ee_block;
		zero_ex.ee_len = cpu_to_le16(ext4_ext_get_actual_len(ex));
		ext4_ext_store_pblock(&zero_ex, ext4_ext_pblock(ex));

		err = ext4_ext_get_access(handle, inode, path + depth);
		if (err)
			goto out;
		ext4_ext_mark_initialized(ex);
		ext4_ext_try_to_merge(handle, inode, path, ex);
		err = ext4_ext_dirty(handle, inode, path + path->p_depth);
		goto out;
	}

	/*
	 * four cases:
	 * 1. split the extent into three extents.
	 * 2. split the extent into two extents, zeroout the first half.
	 * 3. split the extent into two extents, zeroout the second half.
	 * 4. split the extent into two extents with out zeroout.
	 */
	split_map.m_lblk = map->m_lblk;
	split_map.m_len = map->m_len;

	if (max_zeroout && (allocated > map->m_len)) {
		if (allocated <= max_zeroout) {
			/* case 3 */
			zero_ex.ee_block =
					 cpu_to_le32(map->m_lblk);
			zero_ex.ee_len = cpu_to_le16(allocated);
			ext4_ext_store_pblock(&zero_ex,
				ext4_ext_pblock(ex) + map->m_lblk - ee_block);
			err = ext4_ext_zeroout(inode, &zero_ex);
			if (err)
				goto out;
			split_map.m_lblk = map->m_lblk;
			split_map.m_len = allocated;
		} else if (map->m_lblk - ee_block + map->m_len < max_zeroout) {
			/* case 2 */
			if (map->m_lblk != ee_block) {
				zero_ex.ee_block = ex->ee_block;
				zero_ex.ee_len = cpu_to_le16(map->m_lblk -
							ee_block);
				ext4_ext_store_pblock(&zero_ex,
						      ext4_ext_pblock(ex));
				err = ext4_ext_zeroout(inode, &zero_ex);
				if (err)
					goto out;
			}

			split_map.m_lblk = ee_block;
			split_map.m_len = map->m_lblk - ee_block + map->m_len;
			allocated = map->m_len;
		}
	}

	err = ext4_split_extent(handle, inode, ppath, &split_map, split_flag,
				flags);
	if (err > 0)
		err = 0;
out:
	/* If we have gotten a failure, don't zero out status tree */
	if (!err)
		err = ext4_zeroout_es(inode, &zero_ex);
	return err ? err : allocated;
}
