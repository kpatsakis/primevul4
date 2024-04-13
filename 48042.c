ext4_force_split_extent_at(handle_t *handle, struct inode *inode,
			   struct ext4_ext_path **ppath, ext4_lblk_t lblk,
			   int nofail)
{
	struct ext4_ext_path *path = *ppath;
	int unwritten = ext4_ext_is_unwritten(path[path->p_depth].p_ext);

	return ext4_split_extent_at(handle, inode, ppath, lblk, unwritten ?
			EXT4_EXT_MARK_UNWRIT1|EXT4_EXT_MARK_UNWRIT2 : 0,
			EXT4_EX_NOCACHE | EXT4_GET_BLOCKS_PRE_IO |
			(nofail ? EXT4_GET_BLOCKS_METADATA_NOFAIL:0));
}
