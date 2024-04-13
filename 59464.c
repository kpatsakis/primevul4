posix_acl_create(struct inode *dir, umode_t *mode,
		struct posix_acl **default_acl, struct posix_acl **acl)
{
	struct posix_acl *p;
	struct posix_acl *clone;
	int ret;

	*acl = NULL;
	*default_acl = NULL;

	if (S_ISLNK(*mode) || !IS_POSIXACL(dir))
		return 0;

	p = get_acl(dir, ACL_TYPE_DEFAULT);
	if (!p || p == ERR_PTR(-EOPNOTSUPP)) {
		*mode &= ~current_umask();
		return 0;
	}
	if (IS_ERR(p))
		return PTR_ERR(p);

	ret = -ENOMEM;
	clone = posix_acl_clone(p, GFP_NOFS);
	if (!clone)
		goto err_release;

	ret = posix_acl_create_masq(clone, mode);
	if (ret < 0)
		goto err_release_clone;

	if (ret == 0)
		posix_acl_release(clone);
	else
		*acl = clone;

	if (!S_ISDIR(*mode))
		posix_acl_release(p);
	else
		*default_acl = p;

	return 0;

err_release_clone:
	posix_acl_release(clone);
err_release:
	posix_acl_release(p);
	return ret;
}
