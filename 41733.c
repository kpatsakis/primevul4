static int verify_aead(struct nlattr **attrs)
{
	struct nlattr *rt = attrs[XFRMA_ALG_AEAD];
	struct xfrm_algo_aead *algp;

	if (!rt)
		return 0;

	algp = nla_data(rt);
	if (nla_len(rt) < aead_len(algp))
		return -EINVAL;

	algp->alg_name[CRYPTO_MAX_ALG_NAME - 1] = '\0';
	return 0;
}
