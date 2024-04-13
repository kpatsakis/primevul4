static int nfsaclsvc_release_getacl(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd3_getaclres *resp)
{
	fh_put(&resp->fh);
	posix_acl_release(resp->acl_access);
	posix_acl_release(resp->acl_default);
	return 1;
}
