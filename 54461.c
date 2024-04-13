static bool tcmu_logdir_create(const char *path)
{
	DIR* dir = opendir(path);

	if (dir) {
		closedir(dir);
	} else if (errno == ENOENT) {
		if (mkdir(path, 0755) == -1) {
			tcmu_err("mkdir(%s) failed: %m\n", path);
			return FALSE;
		}
	} else {
		tcmu_err("opendir(%s) failed: %m\n", path);
		return FALSE;
	}

	return TRUE;
}
