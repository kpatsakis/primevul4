static int nfsaclsvc_decode_getaclargs(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd3_getaclargs *argp)
{
	p = nfs2svc_decode_fh(p, &argp->fh);
	if (!p)
		return 0;
	argp->mask = ntohl(*p); p++;

	return xdr_argsize_check(rqstp, p);
}
