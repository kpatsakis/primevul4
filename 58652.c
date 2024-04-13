static int path_with_deleted(const struct path *path,
			     const struct path *root,
			     char **buf, int *buflen)
{
	prepend(buf, buflen, "\0", 1);
	if (d_unlinked(path->dentry)) {
		int error = prepend(buf, buflen, " (deleted)", 10);
		if (error)
			return error;
	}

	return prepend_path(path, root, buf, buflen);
}
