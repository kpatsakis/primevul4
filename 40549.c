static inline int aalg_tmpl_set(const struct xfrm_tmpl *t,
				const struct xfrm_algo_desc *d)
{
	unsigned int id = d->desc.sadb_alg_id;

	if (id >= sizeof(t->aalgos) * 8)
		return 0;

	return (t->aalgos >> id) & 1;
}
