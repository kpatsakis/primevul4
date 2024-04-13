static __be32 nfsacld_proc_getattr(struct svc_rqst * rqstp,
		struct nfsd_fhandle *argp, struct nfsd_attrstat *resp)
{
	__be32 nfserr;
	dprintk("nfsd: GETATTR  %s\n", SVCFH_fmt(&argp->fh));

	fh_copy(&resp->fh, &argp->fh);
	nfserr = fh_verify(rqstp, &resp->fh, 0, NFSD_MAY_NOP);
	if (nfserr)
		return nfserr;
	nfserr = fh_getattr(&resp->fh, &resp->stat);
	return nfserr;
}
