static int attach_one_algo(struct xfrm_algo **algpp, u8 *props,
			   struct xfrm_algo_desc *(*get_byname)(const char *, int),
			   struct nlattr *rta)
{
	struct xfrm_algo *p, *ualg;
	struct xfrm_algo_desc *algo;

	if (!rta)
		return 0;

	ualg = nla_data(rta);

	algo = get_byname(ualg->alg_name, 1);
	if (!algo)
		return -ENOSYS;
	*props = algo->desc.sadb_alg_id;

	p = kmemdup(ualg, xfrm_alg_len(ualg), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	strcpy(p->alg_name, algo->name);
	*algpp = p;
	return 0;
}
