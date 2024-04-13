int evm_inode_removexattr(struct dentry *dentry, const char *xattr_name)
{
	return evm_protect_xattr(dentry, xattr_name, NULL, 0);
}
