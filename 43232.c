static int isofs_remount(struct super_block *sb, int *flags, char *data)
{
	sync_filesystem(sb);
	if (!(*flags & MS_RDONLY))
		return -EROFS;
	return 0;
}
