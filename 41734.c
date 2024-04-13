static int verify_auth_trunc(struct nlattr **attrs)
{
	struct nlattr *rt = attrs[XFRMA_ALG_AUTH_TRUNC];
	struct xfrm_algo_auth *algp;

	if (!rt)
		return 0;

	algp = nla_data(rt);
	if (nla_len(rt) < xfrm_alg_auth_len(algp))
		return -EINVAL;

	algp->alg_name[CRYPTO_MAX_ALG_NAME - 1] = '\0';
	return 0;
}
