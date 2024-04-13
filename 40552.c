static int count_ah_combs(const struct xfrm_tmpl *t)
{
	int i, sz = 0;

	for (i = 0; ; i++) {
		const struct xfrm_algo_desc *aalg = xfrm_aalg_get_byidx(i);
		if (!aalg)
			break;
		if (!aalg->pfkey_supported)
			continue;
		if (aalg_tmpl_set(t, aalg) && aalg->available)
			sz += sizeof(struct sadb_comb);
	}
	return sz + sizeof(struct sadb_prop);
}
