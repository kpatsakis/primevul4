static int attach_auth(struct xfrm_algo_auth **algpp, u8 *props,
		       struct nlattr *rta)
{
	struct xfrm_algo *ualg;
	struct xfrm_algo_auth *p;
	struct xfrm_algo_desc *algo;

	if (!rta)
		return 0;

	ualg = nla_data(rta);

	algo = xfrm_aalg_get_byname(ualg->alg_name, 1);
	if (!algo)
		return -ENOSYS;
	*props = algo->desc.sadb_alg_id;

	p = kmalloc(sizeof(*p) + (ualg->alg_key_len + 7) / 8, GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	strcpy(p->alg_name, algo->name);
	p->alg_key_len = ualg->alg_key_len;
	p->alg_trunc_len = algo->uinfo.auth.icv_truncbits;
	memcpy(p->alg_key, ualg->alg_key, (ualg->alg_key_len + 7) / 8);

	*algpp = p;
	return 0;
}
