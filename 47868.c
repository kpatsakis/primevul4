static bool fc_may_access(struct fuse_context *fc, const char *contrl, const char *cg, const char *file, mode_t mode)
{
	struct cgfs_files *k = NULL;
	bool ret = false;

	if (!file)
		file = "tasks";

	if (*file == '/')
		file++;

	k = cgfs_get_key(contrl, cg, file);
	if (!k)
		return false;

	if (is_privileged_over(fc->pid, fc->uid, k->uid, NS_ROOT_OPT)) {
		if (perms_include(k->mode >> 6, mode)) {
			ret = true;
			goto out;
		}
	}
	if (fc->gid == k->gid) {
		if (perms_include(k->mode >> 3, mode)) {
			ret = true;
			goto out;
		}
	}
	ret = perms_include(k->mode, mode);

out:
	free_key(k);
	return ret;
}
