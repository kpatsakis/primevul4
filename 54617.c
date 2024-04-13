int walk_page_range(unsigned long start, unsigned long end,
		    struct mm_walk *walk)
{
	int err = 0;
	unsigned long next;
	struct vm_area_struct *vma;

	if (start >= end)
		return -EINVAL;

	if (!walk->mm)
		return -EINVAL;

	VM_BUG_ON_MM(!rwsem_is_locked(&walk->mm->mmap_sem), walk->mm);

	vma = find_vma(walk->mm, start);
	do {
		if (!vma) { /* after the last vma */
			walk->vma = NULL;
			next = end;
		} else if (start < vma->vm_start) { /* outside vma */
			walk->vma = NULL;
			next = min(end, vma->vm_start);
		} else { /* inside vma */
			walk->vma = vma;
			next = min(end, vma->vm_end);
			vma = vma->vm_next;

			err = walk_page_test(start, next, walk);
			if (err > 0) {
				/*
				 * positive return values are purely for
				 * controlling the pagewalk, so should never
				 * be passed to the callers.
				 */
				err = 0;
				continue;
			}
			if (err < 0)
				break;
		}
		if (walk->vma || walk->pte_hole)
			err = __walk_page_range(start, next, walk);
		if (err)
			break;
	} while (start = next, start < end);
	return err;
}
