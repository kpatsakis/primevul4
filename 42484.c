static int shift_arg_pages(struct vm_area_struct *vma, unsigned long shift)
{
	struct mm_struct *mm = vma->vm_mm;
	unsigned long old_start = vma->vm_start;
	unsigned long old_end = vma->vm_end;
	unsigned long length = old_end - old_start;
	unsigned long new_start = old_start - shift;
	unsigned long new_end = old_end - shift;
	struct mmu_gather tlb;

	BUG_ON(new_start > new_end);

	/*
	 * ensure there are no vmas between where we want to go
	 * and where we are
	 */
	if (vma != find_vma(mm, new_start))
		return -EFAULT;

	/*
	 * cover the whole range: [new_start, old_end)
	 */
	if (vma_adjust(vma, new_start, old_end, vma->vm_pgoff, NULL))
		return -ENOMEM;

	/*
	 * move the page tables downwards, on failure we rely on
	 * process cleanup to remove whatever mess we made.
	 */
	if (length != move_page_tables(vma, old_start,
				       vma, new_start, length, false))
		return -ENOMEM;

	lru_add_drain();
	tlb_gather_mmu(&tlb, mm, 0);
	if (new_end > old_start) {
		/*
		 * when the old and new regions overlap clear from new_end.
		 */
		free_pgd_range(&tlb, new_end, old_end, new_end,
			vma->vm_next ? vma->vm_next->vm_start : 0);
	} else {
		/*
		 * otherwise, clean from old_start; this is done to not touch
		 * the address space in [new_end, old_start) some architectures
		 * have constraints on va-space that make this illegal (IA64) -
		 * for the others its just a little faster.
		 */
		free_pgd_range(&tlb, old_start, old_end, new_end,
			vma->vm_next ? vma->vm_next->vm_start : 0);
	}
	tlb_finish_mmu(&tlb, new_end, old_end);

	/*
	 * Shrink the vma to just the new range.  Always succeeds.
	 */
	vma_adjust(vma, new_start, new_end, vma->vm_pgoff, NULL);

	return 0;
}
