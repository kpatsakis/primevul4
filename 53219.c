static int nfs3svc_decode_setaclargs(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd3_setaclargs *args)
{
	struct kvec *head = rqstp->rq_arg.head;
	unsigned int base;
	int n;

	p = nfs3svc_decode_fh(p, &args->fh);
	if (!p)
		return 0;
	args->mask = ntohl(*p++);
	if (args->mask & ~NFS_ACL_MASK ||
	    !xdr_argsize_check(rqstp, p))
		return 0;

	base = (char *)p - (char *)head->iov_base;
	n = nfsacl_decode(&rqstp->rq_arg, base, NULL,
			  (args->mask & NFS_ACL) ?
			  &args->acl_access : NULL);
	if (n > 0)
		n = nfsacl_decode(&rqstp->rq_arg, base + n, NULL,
				  (args->mask & NFS_DFACL) ?
				  &args->acl_default : NULL);
	return (n > 0);
}
