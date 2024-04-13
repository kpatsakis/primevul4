static int debugfs_remount(struct super_block *sb, int *flags, char *data)
{
	int err;
	struct debugfs_fs_info *fsi = sb->s_fs_info;

	sync_filesystem(sb);
	err = debugfs_parse_options(data, &fsi->mount_opts);
	if (err)
		goto fail;

	debugfs_apply_options(sb);

fail:
	return err;
}
