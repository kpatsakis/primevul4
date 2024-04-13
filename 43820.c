static u64 *rmap_get_next(struct rmap_iterator *iter)
{
	if (iter->desc) {
		if (iter->pos < PTE_LIST_EXT - 1) {
			u64 *sptep;

			++iter->pos;
			sptep = iter->desc->sptes[iter->pos];
			if (sptep)
				return sptep;
		}

		iter->desc = iter->desc->more;

		if (iter->desc) {
			iter->pos = 0;
			/* desc->sptes[0] cannot be NULL */
			return iter->desc->sptes[iter->pos];
		}
	}

	return NULL;
}
