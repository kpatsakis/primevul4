int evm_inode_init_security(struct inode *inode,
				 const struct xattr *lsm_xattr,
				 struct xattr *evm_xattr)
{
	struct evm_ima_xattr_data *xattr_data;
	int rc;

	if (!evm_initialized || !evm_protected_xattr(lsm_xattr->name))
		return 0;

	xattr_data = kzalloc(sizeof(*xattr_data), GFP_NOFS);
	if (!xattr_data)
		return -ENOMEM;

	xattr_data->type = EVM_XATTR_HMAC;
	rc = evm_init_hmac(inode, lsm_xattr, xattr_data->digest);
	if (rc < 0)
		goto out;

	evm_xattr->value = xattr_data;
	evm_xattr->value_len = sizeof(*xattr_data);
	evm_xattr->name = XATTR_EVM_SUFFIX;
	return 0;
out:
	kfree(xattr_data);
	return rc;
}
