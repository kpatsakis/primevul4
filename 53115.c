void evm_inode_post_setxattr(struct dentry *dentry, const char *xattr_name,
			     const void *xattr_value, size_t xattr_value_len)
{
	if (!evm_initialized || (!evm_protected_xattr(xattr_name)
				 && !posix_xattr_acl(xattr_name)))
		return;

	evm_reset_status(dentry->d_inode);

	evm_update_evmxattr(dentry, xattr_name, xattr_value, xattr_value_len);
}
