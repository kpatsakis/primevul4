static int nfsaclsvc_encode_attrstatres(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd_attrstat *resp)
{
	p = nfs2svc_encode_fattr(rqstp, p, &resp->fh, &resp->stat);
	return xdr_ressize_check(rqstp, p);
}
