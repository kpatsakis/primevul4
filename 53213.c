static int nfsaclsvc_encode_getaclres(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd3_getaclres *resp)
{
	struct dentry *dentry = resp->fh.fh_dentry;
	struct inode *inode;
	struct kvec *head = rqstp->rq_res.head;
	unsigned int base;
	int n;
	int w;

	/*
	 * Since this is version 2, the check for nfserr in
	 * nfsd_dispatch actually ensures the following cannot happen.
	 * However, it seems fragile to depend on that.
	 */
	if (dentry == NULL || d_really_is_negative(dentry))
		return 0;
	inode = d_inode(dentry);

	p = nfs2svc_encode_fattr(rqstp, p, &resp->fh, &resp->stat);
	*p++ = htonl(resp->mask);
	if (!xdr_ressize_check(rqstp, p))
		return 0;
	base = (char *)p - (char *)head->iov_base;

	rqstp->rq_res.page_len = w = nfsacl_size(
		(resp->mask & NFS_ACL)   ? resp->acl_access  : NULL,
		(resp->mask & NFS_DFACL) ? resp->acl_default : NULL);
	while (w > 0) {
		if (!*(rqstp->rq_next_page++))
			return 0;
		w -= PAGE_SIZE;
	}

	n = nfsacl_encode(&rqstp->rq_res, base, inode,
			  resp->acl_access,
			  resp->mask & NFS_ACL, 0);
	if (n > 0)
		n = nfsacl_encode(&rqstp->rq_res, base + n, inode,
				  resp->acl_default,
				  resp->mask & NFS_DFACL,
				  NFS_ACL_DEFAULT);
	return (n > 0);
}
