enum integrity_status evm_verifyxattr(struct dentry *dentry,
				      const char *xattr_name,
				      void *xattr_value, size_t xattr_value_len,
				      struct integrity_iint_cache *iint)
{
	if (!evm_initialized || !evm_protected_xattr(xattr_name))
		return INTEGRITY_UNKNOWN;

	if (!iint) {
		iint = integrity_iint_find(d_backing_inode(dentry));
		if (!iint)
			return INTEGRITY_UNKNOWN;
	}
	return evm_verify_hmac(dentry, xattr_name, xattr_value,
				 xattr_value_len, iint);
}
