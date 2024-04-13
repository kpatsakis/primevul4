static int cg_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
		struct fuse_file_info *fi)
{
	struct file_info *d = (struct file_info *)fi->fh;
	struct cgfs_files **list = NULL;
	int i, ret;
	char *nextcg = NULL;
	struct fuse_context *fc = fuse_get_context();
	char **clist = NULL;

	if (d->type != LXC_TYPE_CGDIR) {
		fprintf(stderr, "Internal error: file cache info used in readdir\n");
		return -EIO;
	}
	if (!d->cgroup && !d->controller) {
		int i;

		for (i = 0;  i < num_hierarchies; i++) {
			if (hierarchies[i] && filler(buf, hierarchies[i], NULL, 0) != 0) {
				return -EIO;
			}
		}
		return 0;
	}

	if (!cgfs_list_keys(d->controller, d->cgroup, &list)) {
		ret = -EINVAL;
		goto out;
	}

	if (!caller_is_in_ancestor(fc->pid, d->controller, d->cgroup, &nextcg)) {
		if (nextcg) {
			int ret;
			ret = filler(buf, nextcg,  NULL, 0);
			free(nextcg);
			if (ret != 0) {
				ret = -EIO;
				goto out;
			}
		}
		ret = 0;
		goto out;
	}

	for (i = 0; list[i]; i++) {
		if (filler(buf, list[i]->name, NULL, 0) != 0) {
			ret = -EIO;
			goto out;
		}
	}


	if (!cgfs_list_children(d->controller, d->cgroup, &clist)) {
		ret = 0;
		goto out;
	}
	for (i = 0; clist[i]; i++) {
		if (filler(buf, clist[i], NULL, 0) != 0) {
			ret = -EIO;
			goto out;
		}
	}
	ret = 0;

out:
	free_keys(list);
	if (clist) {
		for (i = 0; clist[i]; i++)
			free(clist[i]);
		free(clist);
	}
	return ret;
}
