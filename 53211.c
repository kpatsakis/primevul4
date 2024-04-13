static int nfsaclsvc_decode_setaclargs(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd3_setaclargs *argp)
{
	struct kvec *head = rqstp->rq_arg.head;
	unsigned int base;
	int n;

	p = nfs2svc_decode_fh(p, &argp->fh);
	if (!p)
		return 0;
	argp->mask = ntohl(*p++);
	if (argp->mask & ~NFS_ACL_MASK ||
	    !xdr_argsize_check(rqstp, p))
		return 0;

	base = (char *)p - (char *)head->iov_base;
	n = nfsacl_decode(&rqstp->rq_arg, base, NULL,
			  (argp->mask & NFS_ACL) ?
			  &argp->acl_access : NULL);
	if (n > 0)
		n = nfsacl_decode(&rqstp->rq_arg, base + n, NULL,
				  (argp->mask & NFS_DFACL) ?
				  &argp->acl_default : NULL);
	return (n > 0);
}
