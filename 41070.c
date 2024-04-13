static inline unsigned dx_node_limit(struct inode *dir)
{
	unsigned entry_space = dir->i_sb->s_blocksize - EXT4_DIR_REC_LEN(0);

	if (EXT4_HAS_RO_COMPAT_FEATURE(dir->i_sb,
				       EXT4_FEATURE_RO_COMPAT_METADATA_CSUM))
		entry_space -= sizeof(struct dx_tail);
	return entry_space / sizeof(struct dx_entry);
}
