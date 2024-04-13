nfsd4_get_nfs4_acl(struct svc_rqst *rqstp, struct dentry *dentry,
		struct nfs4_acl **acl)
{
	struct inode *inode = d_inode(dentry);
	int error = 0;
	struct posix_acl *pacl = NULL, *dpacl = NULL;
	unsigned int flags = 0;
	int size = 0;

	pacl = get_acl(inode, ACL_TYPE_ACCESS);
	if (!pacl)
		pacl = posix_acl_from_mode(inode->i_mode, GFP_KERNEL);

	if (IS_ERR(pacl))
		return PTR_ERR(pacl);

	/* allocate for worst case: one (deny, allow) pair each: */
	size += 2 * pacl->a_count;

	if (S_ISDIR(inode->i_mode)) {
		flags = NFS4_ACL_DIR;
		dpacl = get_acl(inode, ACL_TYPE_DEFAULT);
		if (IS_ERR(dpacl)) {
			error = PTR_ERR(dpacl);
			goto rel_pacl;
		}

		if (dpacl)
			size += 2 * dpacl->a_count;
	}

	*acl = kmalloc(nfs4_acl_bytes(size), GFP_KERNEL);
	if (*acl == NULL) {
		error = -ENOMEM;
		goto out;
	}
	(*acl)->naces = 0;

	_posix_to_nfsv4_one(pacl, *acl, flags & ~NFS4_ACL_TYPE_DEFAULT);

	if (dpacl)
		_posix_to_nfsv4_one(dpacl, *acl, flags | NFS4_ACL_TYPE_DEFAULT);

out:
	posix_acl_release(dpacl);
rel_pacl:
	posix_acl_release(pacl);
	return error;
}
