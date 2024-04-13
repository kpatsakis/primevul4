static int evm_calc_hmac_or_hash(struct dentry *dentry,
				const char *req_xattr_name,
				const char *req_xattr_value,
				size_t req_xattr_value_len,
				char type, char *digest)
{
	struct inode *inode = dentry->d_inode;
	struct shash_desc *desc;
	char **xattrname;
	size_t xattr_size = 0;
	char *xattr_value = NULL;
	int error;
	int size;

	if (!inode->i_op || !inode->i_op->getxattr)
		return -EOPNOTSUPP;
	desc = init_desc(type);
	if (IS_ERR(desc))
		return PTR_ERR(desc);

	error = -ENODATA;
	for (xattrname = evm_config_xattrnames; *xattrname != NULL; xattrname++) {
		if ((req_xattr_name && req_xattr_value)
		    && !strcmp(*xattrname, req_xattr_name)) {
			error = 0;
			crypto_shash_update(desc, (const u8 *)req_xattr_value,
					     req_xattr_value_len);
			continue;
		}
		size = vfs_getxattr_alloc(dentry, *xattrname,
					  &xattr_value, xattr_size, GFP_NOFS);
		if (size == -ENOMEM) {
			error = -ENOMEM;
			goto out;
		}
		if (size < 0)
			continue;

		error = 0;
		xattr_size = size;
		crypto_shash_update(desc, (const u8 *)xattr_value, xattr_size);
	}
	hmac_add_misc(desc, inode, digest);

out:
	kfree(xattr_value);
	kfree(desc);
	return error;
}
