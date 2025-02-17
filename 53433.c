int ovl_copy_xattr(struct dentry *old, struct dentry *new)
{
	ssize_t list_size, size;
	char *buf, *name, *value;
	int error;

	if (!old->d_inode->i_op->getxattr ||
	    !new->d_inode->i_op->getxattr)
		return 0;

	list_size = vfs_listxattr(old, NULL, 0);
	if (list_size <= 0) {
		if (list_size == -EOPNOTSUPP)
			return 0;
		return list_size;
	}

	buf = kzalloc(list_size, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	error = -ENOMEM;
	value = kmalloc(XATTR_SIZE_MAX, GFP_KERNEL);
	if (!value)
		goto out;

	list_size = vfs_listxattr(old, buf, list_size);
	if (list_size <= 0) {
		error = list_size;
		goto out_free_value;
	}

	for (name = buf; name < (buf + list_size); name += strlen(name) + 1) {
		size = vfs_getxattr(old, name, value, XATTR_SIZE_MAX);
		if (size <= 0) {
			error = size;
			goto out_free_value;
		}
		error = vfs_setxattr(new, name, value, size, 0);
		if (error)
			goto out_free_value;
	}

out_free_value:
	kfree(value);
out:
	kfree(buf);
	return error;
}
