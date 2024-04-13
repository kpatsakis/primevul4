static int ext4_convert_unwritten_extents_endio(handle_t *handle,
						struct inode *inode,
						struct ext4_map_blocks *map,
						struct ext4_ext_path **ppath)
{
	struct ext4_ext_path *path = *ppath;
	struct ext4_extent *ex;
	ext4_lblk_t ee_block;
	unsigned int ee_len;
	int depth;
	int err = 0;

	depth = ext_depth(inode);
	ex = path[depth].p_ext;
	ee_block = le32_to_cpu(ex->ee_block);
	ee_len = ext4_ext_get_actual_len(ex);

	ext_debug("ext4_convert_unwritten_extents_endio: inode %lu, logical"
		"block %llu, max_blocks %u\n", inode->i_ino,
		  (unsigned long long)ee_block, ee_len);

	/* If extent is larger than requested it is a clear sign that we still
	 * have some extent state machine issues left. So extent_split is still
	 * required.
	 * TODO: Once all related issues will be fixed this situation should be
	 * illegal.
	 */
	if (ee_block != map->m_lblk || ee_len > map->m_len) {
#ifdef EXT4_DEBUG
		ext4_warning("Inode (%ld) finished: extent logical block %llu,"
			     " len %u; IO logical block %llu, len %u\n",
			     inode->i_ino, (unsigned long long)ee_block, ee_len,
			     (unsigned long long)map->m_lblk, map->m_len);
#endif
		err = ext4_split_convert_extents(handle, inode, map, ppath,
						 EXT4_GET_BLOCKS_CONVERT);
		if (err < 0)
			return err;
		path = ext4_find_extent(inode, map->m_lblk, ppath, 0);
		if (IS_ERR(path))
			return PTR_ERR(path);
		depth = ext_depth(inode);
		ex = path[depth].p_ext;
	}

	err = ext4_ext_get_access(handle, inode, path + depth);
	if (err)
		goto out;
	/* first mark the extent as initialized */
	ext4_ext_mark_initialized(ex);

	/* note: ext4_ext_correct_indexes() isn't needed here because
	 * borders are not changed
	 */
	ext4_ext_try_to_merge(handle, inode, path, ex);

	/* Mark modified extent as dirty */
	err = ext4_ext_dirty(handle, inode, path + path->p_depth);
out:
	ext4_ext_show_leaf(inode, path);
	return err;
}
