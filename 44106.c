static int do_mlockall(int flags)
{
	struct vm_area_struct * vma, * prev = NULL;

	if (flags & MCL_FUTURE)
		current->mm->def_flags |= VM_LOCKED;
	else
		current->mm->def_flags &= ~VM_LOCKED;
	if (flags == MCL_FUTURE)
		goto out;

	for (vma = current->mm->mmap; vma ; vma = prev->vm_next) {
		vm_flags_t newflags;

		newflags = vma->vm_flags & ~VM_LOCKED;
		if (flags & MCL_CURRENT)
			newflags |= VM_LOCKED;

		/* Ignore errors */
		mlock_fixup(vma, &prev, vma->vm_start, vma->vm_end, newflags);
		cond_resched();
	}
out:
	return 0;
}
