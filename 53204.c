static __be32 nfsacld_proc_access(struct svc_rqst *rqstp, struct nfsd3_accessargs *argp,
		struct nfsd3_accessres *resp)
{
	__be32 nfserr;

	dprintk("nfsd: ACCESS(2acl)   %s 0x%x\n",
			SVCFH_fmt(&argp->fh),
			argp->access);

	fh_copy(&resp->fh, &argp->fh);
	resp->access = argp->access;
	nfserr = nfsd_access(rqstp, &resp->fh, &resp->access, NULL);
	if (nfserr)
		return nfserr;
	nfserr = fh_getattr(&resp->fh, &resp->stat);
	return nfserr;
}
