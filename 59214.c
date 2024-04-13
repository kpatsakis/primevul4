static unsigned long perf_addr_filter_apply(struct perf_addr_filter *filter,
					    struct mm_struct *mm)
{
	struct vm_area_struct *vma;

	for (vma = mm->mmap; vma; vma = vma->vm_next) {
		struct file *file = vma->vm_file;
		unsigned long off = vma->vm_pgoff << PAGE_SHIFT;
		unsigned long vma_size = vma->vm_end - vma->vm_start;

		if (!file)
			continue;

		if (!perf_addr_filter_match(filter, file, off, vma_size))
			continue;

		return vma->vm_start;
	}

	return 0;
}
