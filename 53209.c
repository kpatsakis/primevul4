static int nfsaclsvc_decode_fhandleargs(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd_fhandle *argp)
{
	p = nfs2svc_decode_fh(p, &argp->fh);
	if (!p)
		return 0;
	return xdr_argsize_check(rqstp, p);
}
