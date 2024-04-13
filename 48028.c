static void ext4_ext_show_leaf(struct inode *inode, struct ext4_ext_path *path)
{
	int depth = ext_depth(inode);
	struct ext4_extent_header *eh;
	struct ext4_extent *ex;
	int i;

	if (!path)
		return;

	eh = path[depth].p_hdr;
	ex = EXT_FIRST_EXTENT(eh);

	ext_debug("Displaying leaf extents for inode %lu\n", inode->i_ino);

	for (i = 0; i < le16_to_cpu(eh->eh_entries); i++, ex++) {
		ext_debug("%d:[%d]%d:%llu ", le32_to_cpu(ex->ee_block),
			  ext4_ext_is_unwritten(ex),
			  ext4_ext_get_actual_len(ex), ext4_ext_pblock(ex));
	}
	ext_debug("\n");
}
