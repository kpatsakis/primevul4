static __be32 nfsacld_proc_getacl(struct svc_rqst * rqstp,
		struct nfsd3_getaclargs *argp, struct nfsd3_getaclres *resp)
{
	struct posix_acl *acl;
	struct inode *inode;
	svc_fh *fh;
	__be32 nfserr = 0;

	dprintk("nfsd: GETACL(2acl)   %s\n", SVCFH_fmt(&argp->fh));

	fh = fh_copy(&resp->fh, &argp->fh);
	nfserr = fh_verify(rqstp, &resp->fh, 0, NFSD_MAY_NOP);
	if (nfserr)
		RETURN_STATUS(nfserr);

	inode = d_inode(fh->fh_dentry);

	if (argp->mask & ~NFS_ACL_MASK)
		RETURN_STATUS(nfserr_inval);
	resp->mask = argp->mask;

	nfserr = fh_getattr(fh, &resp->stat);
	if (nfserr)
		RETURN_STATUS(nfserr);

	if (resp->mask & (NFS_ACL|NFS_ACLCNT)) {
		acl = get_acl(inode, ACL_TYPE_ACCESS);
		if (acl == NULL) {
			/* Solaris returns the inode's minimum ACL. */
			acl = posix_acl_from_mode(inode->i_mode, GFP_KERNEL);
		}
		if (IS_ERR(acl)) {
			nfserr = nfserrno(PTR_ERR(acl));
			goto fail;
		}
		resp->acl_access = acl;
	}
	if (resp->mask & (NFS_DFACL|NFS_DFACLCNT)) {
		/* Check how Solaris handles requests for the Default ACL
		   of a non-directory! */
		acl = get_acl(inode, ACL_TYPE_DEFAULT);
		if (IS_ERR(acl)) {
			nfserr = nfserrno(PTR_ERR(acl));
			goto fail;
		}
		resp->acl_default = acl;
	}

	/* resp->acl_{access,default} are released in nfssvc_release_getacl. */
	RETURN_STATUS(0);

fail:
	posix_acl_release(resp->acl_access);
	posix_acl_release(resp->acl_default);
	RETURN_STATUS(nfserr);
}
