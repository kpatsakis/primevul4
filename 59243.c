void perf_event_mmap(struct vm_area_struct *vma)
{
	struct perf_mmap_event mmap_event;

	if (!atomic_read(&nr_mmap_events))
		return;

	mmap_event = (struct perf_mmap_event){
		.vma	= vma,
		/* .file_name */
		/* .file_size */
		.event_id  = {
			.header = {
				.type = PERF_RECORD_MMAP,
				.misc = PERF_RECORD_MISC_USER,
				/* .size */
			},
			/* .pid */
			/* .tid */
			.start  = vma->vm_start,
			.len    = vma->vm_end - vma->vm_start,
			.pgoff  = (u64)vma->vm_pgoff << PAGE_SHIFT,
		},
		/* .maj (attr_mmap2 only) */
		/* .min (attr_mmap2 only) */
		/* .ino (attr_mmap2 only) */
		/* .ino_generation (attr_mmap2 only) */
		/* .prot (attr_mmap2 only) */
		/* .flags (attr_mmap2 only) */
	};

	perf_addr_filters_adjust(vma);
	perf_event_mmap_event(&mmap_event);
}
