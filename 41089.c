static void ext4_dirent_csum_set(struct inode *inode,
				 struct ext4_dir_entry *dirent)
{
	struct ext4_dir_entry_tail *t;

	if (!EXT4_HAS_RO_COMPAT_FEATURE(inode->i_sb,
					EXT4_FEATURE_RO_COMPAT_METADATA_CSUM))
		return;

	t = get_dirent_tail(inode, dirent);
	if (!t) {
		warn_no_space_for_csum(inode);
		return;
	}

	t->det_checksum = ext4_dirent_csum(inode, dirent,
					   (void *)t - (void *)dirent);
}
