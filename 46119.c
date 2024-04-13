int ovl_setxattr(struct dentry *dentry, const char *name,
		 const void *value, size_t size, int flags)
{
	int err;
	struct dentry *upperdentry;

	err = ovl_want_write(dentry);
	if (err)
		goto out;

	err = -EPERM;
	if (ovl_is_private_xattr(name))
		goto out_drop_write;

	err = ovl_copy_up(dentry);
	if (err)
		goto out_drop_write;

	upperdentry = ovl_dentry_upper(dentry);
	err = vfs_setxattr(upperdentry, name, value, size, flags);

out_drop_write:
	ovl_drop_write(dentry);
out:
	return err;
}
