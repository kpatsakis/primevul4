convert_initialized_extent(handle_t *handle, struct inode *inode,
			   struct ext4_map_blocks *map,
			   struct ext4_ext_path **ppath, int flags,
			   unsigned int allocated, ext4_fsblk_t newblock)
{
	struct ext4_ext_path *path = *ppath;
	struct ext4_extent *ex;
	ext4_lblk_t ee_block;
	unsigned int ee_len;
	int depth;
	int err = 0;

	/*
	 * Make sure that the extent is no bigger than we support with
	 * unwritten extent
	 */
	if (map->m_len > EXT_UNWRITTEN_MAX_LEN)
		map->m_len = EXT_UNWRITTEN_MAX_LEN / 2;

	depth = ext_depth(inode);
	ex = path[depth].p_ext;
	ee_block = le32_to_cpu(ex->ee_block);
	ee_len = ext4_ext_get_actual_len(ex);

	ext_debug("%s: inode %lu, logical"
		"block %llu, max_blocks %u\n", __func__, inode->i_ino,
		  (unsigned long long)ee_block, ee_len);

	if (ee_block != map->m_lblk || ee_len > map->m_len) {
		err = ext4_split_convert_extents(handle, inode, map, ppath,
				EXT4_GET_BLOCKS_CONVERT_UNWRITTEN);
		if (err < 0)
			return err;
		path = ext4_find_extent(inode, map->m_lblk, ppath, 0);
		if (IS_ERR(path))
			return PTR_ERR(path);
		depth = ext_depth(inode);
		ex = path[depth].p_ext;
		if (!ex) {
			EXT4_ERROR_INODE(inode, "unexpected hole at %lu",
					 (unsigned long) map->m_lblk);
			return -EIO;
		}
	}

	err = ext4_ext_get_access(handle, inode, path + depth);
	if (err)
		return err;
	/* first mark the extent as unwritten */
	ext4_ext_mark_unwritten(ex);

	/* note: ext4_ext_correct_indexes() isn't needed here because
	 * borders are not changed
	 */
	ext4_ext_try_to_merge(handle, inode, path, ex);

	/* Mark modified extent as dirty */
	err = ext4_ext_dirty(handle, inode, path + path->p_depth);
	if (err)
		return err;
	ext4_ext_show_leaf(inode, path);

	ext4_update_inode_fsync_trans(handle, inode, 1);
	err = check_eofblocks_fl(handle, inode, map->m_lblk, path, map->m_len);
	if (err)
		return err;
	map->m_flags |= EXT4_MAP_UNWRITTEN;
	if (allocated > map->m_len)
		allocated = map->m_len;
	map->m_len = allocated;
	return allocated;
}
