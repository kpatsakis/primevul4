static int lxcfs_getattr(const char *path, struct stat *sb)
{
	if (strcmp(path, "/") == 0) {
		sb->st_mode = S_IFDIR | 00755;
		sb->st_nlink = 2;
		return 0;
	}
	if (strncmp(path, "/cgroup", 7) == 0) {
		return cg_getattr(path, sb);
	}
	if (strncmp(path, "/proc", 5) == 0) {
		return proc_getattr(path, sb);
	}
	return -EINVAL;
}
