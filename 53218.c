static int nfs3svc_decode_getaclargs(struct svc_rqst *rqstp, __be32 *p,
		struct nfsd3_getaclargs *args)
{
	p = nfs3svc_decode_fh(p, &args->fh);
	if (!p)
		return 0;
	args->mask = ntohl(*p); p++;

	return xdr_argsize_check(rqstp, p);
}
