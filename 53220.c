static int nfs3svc_encode_getaclres(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd3_getaclres *resp)
{
	struct dentry *dentry = resp->fh.fh_dentry;

	p = nfs3svc_encode_post_op_attr(rqstp, p, &resp->fh);
	if (resp->status == 0 && dentry && d_really_is_positive(dentry)) {
		struct inode *inode = d_inode(dentry);
		struct kvec *head = rqstp->rq_res.head;
		unsigned int base;
		int n;
		int w;

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
		if (n <= 0)
			return 0;
	} else
		if (!xdr_ressize_check(rqstp, p))
			return 0;

	return 1;
}
