static void pte_list_walk(unsigned long *pte_list, pte_list_walk_fn fn)
{
	struct pte_list_desc *desc;
	int i;

	if (!*pte_list)
		return;

	if (!(*pte_list & 1))
		return fn((u64 *)*pte_list);

	desc = (struct pte_list_desc *)(*pte_list & ~1ul);
	while (desc) {
		for (i = 0; i < PTE_LIST_EXT && desc->sptes[i]; ++i)
			fn(desc->sptes[i]);
		desc = desc->more;
	}
}
