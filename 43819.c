static u64 *rmap_get_first(unsigned long rmap, struct rmap_iterator *iter)
{
	if (!rmap)
		return NULL;

	if (!(rmap & 1)) {
		iter->desc = NULL;
		return (u64 *)rmap;
	}

	iter->desc = (struct pte_list_desc *)(rmap & ~1ul);
	iter->pos = 0;
	return iter->desc->sptes[iter->pos];
}
