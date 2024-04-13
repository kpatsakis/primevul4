int cg_chown(const char *path, uid_t uid, gid_t gid)
{
	struct fuse_context *fc = fuse_get_context();
	char *cgdir = NULL, *fpath = NULL, *path1, *path2, *controller;
	struct cgfs_files *k = NULL;
	const char *cgroup;
	int ret;

	if (!fc)
		return -EIO;

	if (strcmp(path, "/cgroup") == 0)
		return -EINVAL;

	controller = pick_controller_from_path(fc, path);
	if (!controller)
		return -EINVAL;
	cgroup = find_cgroup_in_path(path);
	if (!cgroup)
		/* this is just /cgroup/controller */
		return -EINVAL;

	get_cgdir_and_path(cgroup, &cgdir, &fpath);

	if (!fpath) {
		path1 = "/";
		path2 = cgdir;
	} else {
		path1 = cgdir;
		path2 = fpath;
	}

	if (is_child_cgroup(controller, path1, path2)) {
		k = cgfs_get_key(controller, cgroup, "tasks");

	} else
		k = cgfs_get_key(controller, path1, path2);

	if (!k) {
		ret = -EINVAL;
		goto out;
	}

	/*
	 * This being a fuse request, the uid and gid must be valid
	 * in the caller's namespace.  So we can just check to make
	 * sure that the caller is root in his uid, and privileged
	 * over the file's current owner.
	 */
	if (!is_privileged_over(fc->pid, fc->uid, k->uid, NS_ROOT_REQD)) {
		ret = -EACCES;
		goto out;
	}

	ret = cgfs_chown_file(controller, cgroup, uid, gid);

out:
	free_key(k);
	free(cgdir);

	return ret;
}
