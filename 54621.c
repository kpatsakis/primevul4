static int walk_pud_range(p4d_t *p4d, unsigned long addr, unsigned long end,
			  struct mm_walk *walk)
{
	pud_t *pud;
	unsigned long next;
	int err = 0;

	pud = pud_offset(p4d, addr);
	do {
 again:
		next = pud_addr_end(addr, end);
		if (pud_none(*pud) || !walk->vma) {
			if (walk->pte_hole)
				err = walk->pte_hole(addr, next, walk);
			if (err)
				break;
			continue;
		}

		if (walk->pud_entry) {
			spinlock_t *ptl = pud_trans_huge_lock(pud, walk->vma);

			if (ptl) {
				err = walk->pud_entry(pud, addr, next, walk);
				spin_unlock(ptl);
				if (err)
					break;
				continue;
			}
		}

		split_huge_pud(walk->vma, pud, addr);
		if (pud_none(*pud))
			goto again;

		if (walk->pmd_entry || walk->pte_entry)
			err = walk_pmd_range(pud, addr, next, walk);
		if (err)
			break;
	} while (pud++, addr = next, addr != end);

	return err;
}
