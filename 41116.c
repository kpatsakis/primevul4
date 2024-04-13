static inline void ext4_set_de_type(struct super_block *sb,
				struct ext4_dir_entry_2 *de,
				umode_t mode) {
	if (EXT4_HAS_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_FILETYPE))
		de->file_type = ext4_type_by_mode[(mode & S_IFMT)>>S_SHIFT];
}
