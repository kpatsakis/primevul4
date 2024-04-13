static int nfsaclsvc_decode_accessargs(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd3_accessargs *argp)
{
	p = nfs2svc_decode_fh(p, &argp->fh);
	if (!p)
		return 0;
	argp->access = ntohl(*p++);

	return xdr_argsize_check(rqstp, p);
}
