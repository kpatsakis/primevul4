int evm_inode_setattr(struct dentry *dentry, struct iattr *attr)
{
	unsigned int ia_valid = attr->ia_valid;
	enum integrity_status evm_status;

	if (!(ia_valid & (ATTR_MODE | ATTR_UID | ATTR_GID)))
		return 0;
	evm_status = evm_verify_current_integrity(dentry);
	if ((evm_status == INTEGRITY_PASS) ||
	    (evm_status == INTEGRITY_NOXATTRS))
		return 0;
	integrity_audit_msg(AUDIT_INTEGRITY_METADATA, d_backing_inode(dentry),
			    dentry->d_name.name, "appraise_metadata",
			    integrity_status_msg[evm_status], -EPERM, 0);
	return -EPERM;
}
