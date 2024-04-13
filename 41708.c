static int attach_aead(struct xfrm_algo_aead **algpp, u8 *props,
		       struct nlattr *rta)
{
	struct xfrm_algo_aead *p, *ualg;
	struct xfrm_algo_desc *algo;

	if (!rta)
		return 0;

	ualg = nla_data(rta);

	algo = xfrm_aead_get_byname(ualg->alg_name, ualg->alg_icv_len, 1);
	if (!algo)
		return -ENOSYS;
	*props = algo->desc.sadb_alg_id;

	p = kmemdup(ualg, aead_len(ualg), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	strcpy(p->alg_name, algo->name);
	*algpp = p;
	return 0;
}
