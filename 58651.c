static enum d_walk_ret path_check_mount(void *data, struct dentry *dentry)
{
	struct check_mount *info = data;
	struct path path = { .mnt = info->mnt, .dentry = dentry };

	if (likely(!d_mountpoint(dentry)))
		return D_WALK_CONTINUE;
	if (__path_is_mountpoint(&path)) {
		info->mounted = 1;
		return D_WALK_QUIT;
	}
	return D_WALK_CONTINUE;
}
