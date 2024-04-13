static enum integrity_status evm_verify_current_integrity(struct dentry *dentry)
{
	struct inode *inode = d_backing_inode(dentry);

	if (!evm_initialized || !S_ISREG(inode->i_mode) || evm_fixmode)
		return 0;
	return evm_verify_hmac(dentry, NULL, NULL, 0, NULL);
}
