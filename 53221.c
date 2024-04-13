static int nfs3svc_encode_setaclres(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd3_attrstat *resp)
{
	p = nfs3svc_encode_post_op_attr(rqstp, p, &resp->fh);

	return xdr_ressize_check(rqstp, p);
}
