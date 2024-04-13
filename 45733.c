static ssize_t sockfs_getxattr(struct dentry *dentry,
			       const char *name, void *value, size_t size)
{
	const char *proto_name;
	size_t proto_size;
	int error;

	error = -ENODATA;
	if (!strncmp(name, XATTR_NAME_SOCKPROTONAME, XATTR_NAME_SOCKPROTONAME_LEN)) {
		proto_name = dentry->d_name.name;
		proto_size = strlen(proto_name);

		if (value) {
			error = -ERANGE;
			if (proto_size + 1 > size)
				goto out;

			strncpy(value, proto_name, proto_size + 1);
		}
		error = proto_size + 1;
	}

out:
	return error;
}
