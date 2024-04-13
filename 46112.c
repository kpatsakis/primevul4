ssize_t ovl_listxattr(struct dentry *dentry, char *list, size_t size)
{
	struct path realpath;
	enum ovl_path_type type = ovl_path_real(dentry, &realpath);
	ssize_t res;
	int off;

	res = vfs_listxattr(realpath.dentry, list, size);
	if (res <= 0 || size == 0)
		return res;

	if (!ovl_need_xattr_filter(dentry, type))
		return res;

	/* filter out private xattrs */
	for (off = 0; off < res;) {
		char *s = list + off;
		size_t slen = strlen(s) + 1;

		BUG_ON(off + slen > res);

		if (ovl_is_private_xattr(s)) {
			res -= slen;
			memmove(s, s + slen, res - off);
		} else {
			off += slen;
		}
	}

	return res;
}
