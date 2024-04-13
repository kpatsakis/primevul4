void evm_inode_post_removexattr(struct dentry *dentry, const char *xattr_name)
{
	if (!evm_initialized || !evm_protected_xattr(xattr_name))
		return;

	evm_reset_status(dentry->d_inode);

	evm_update_evmxattr(dentry, xattr_name, NULL, 0);
}
