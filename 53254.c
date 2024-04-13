static void huge_pte_to_pagemap_entry(pagemap_entry_t *pme, struct pagemapread *pm,
					pte_t pte, int offset, int flags2)
{
	if (pte_present(pte))
		*pme = make_pme(PM_PFRAME(pte_pfn(pte) + offset)	|
				PM_STATUS2(pm->v2, flags2)		|
				PM_PRESENT);
	else
		*pme = make_pme(PM_NOT_PRESENT(pm->v2)			|
				PM_STATUS2(pm->v2, flags2));
}
