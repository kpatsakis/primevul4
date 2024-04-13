pte_list_desc_remove_entry(unsigned long *pte_list, struct pte_list_desc *desc,
			   int i, struct pte_list_desc *prev_desc)
{
	int j;

	for (j = PTE_LIST_EXT - 1; !desc->sptes[j] && j > i; --j)
		;
	desc->sptes[i] = desc->sptes[j];
	desc->sptes[j] = NULL;
	if (j != 0)
		return;
	if (!prev_desc && !desc->more)
		*pte_list = (unsigned long)desc->sptes[0];
	else
		if (prev_desc)
			prev_desc->more = desc->more;
		else
			*pte_list = (unsigned long)desc->more | 1;
	mmu_free_pte_list_desc(desc);
}
