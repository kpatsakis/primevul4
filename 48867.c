int jfs_setattr(struct dentry *dentry, struct iattr *iattr)
{
	struct inode *inode = dentry->d_inode;
	int rc;

	rc = inode_change_ok(inode, iattr);
	if (rc)
		return rc;

	if (is_quota_modification(inode, iattr))
		dquot_initialize(inode);
	if ((iattr->ia_valid & ATTR_UID && !uid_eq(iattr->ia_uid, inode->i_uid)) ||
	    (iattr->ia_valid & ATTR_GID && !gid_eq(iattr->ia_gid, inode->i_gid))) {
		rc = dquot_transfer(inode, iattr);
		if (rc)
			return rc;
	}

	if ((iattr->ia_valid & ATTR_SIZE) &&
	    iattr->ia_size != i_size_read(inode)) {
		inode_dio_wait(inode);

		rc = inode_newsize_ok(inode, iattr->ia_size);
		if (rc)
			return rc;

		truncate_setsize(inode, iattr->ia_size);
		jfs_truncate(inode);
	}

	setattr_copy(inode, iattr);
	mark_inode_dirty(inode);

	if (iattr->ia_valid & ATTR_MODE)
		rc = posix_acl_chmod(inode, inode->i_mode);
	return rc;
}
