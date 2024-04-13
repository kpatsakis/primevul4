ffs_fs_kill_sb(struct super_block *sb)
{
	ENTER();

	kill_litter_super(sb);
	if (sb->s_fs_info) {
		ffs_release_dev(sb->s_fs_info);
		ffs_data_closed(sb->s_fs_info);
		ffs_data_put(sb->s_fs_info);
	}
}
