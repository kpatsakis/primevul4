static int nfsaclsvc_release_attrstat(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd_attrstat *resp)
{
	fh_put(&resp->fh);
	return 1;
}
