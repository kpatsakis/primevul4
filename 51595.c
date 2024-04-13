heap_add_entry(struct archive_read *a, struct heap_queue *heap,
    struct file_info *file, uint64_t key)
{
	uint64_t file_key, parent_key;
	int hole, parent;

	/* Expand our pending files list as necessary. */
	if (heap->used >= heap->allocated) {
		struct file_info **new_pending_files;
		int new_size = heap->allocated * 2;

		if (heap->allocated < 1024)
			new_size = 1024;
		/* Overflow might keep us from growing the list. */
		if (new_size <= heap->allocated) {
			archive_set_error(&a->archive,
			    ENOMEM, "Out of memory");
			return (ARCHIVE_FATAL);
		}
		new_pending_files = (struct file_info **)
		    malloc(new_size * sizeof(new_pending_files[0]));
		if (new_pending_files == NULL) {
			archive_set_error(&a->archive,
			    ENOMEM, "Out of memory");
			return (ARCHIVE_FATAL);
		}
		memcpy(new_pending_files, heap->files,
		    heap->allocated * sizeof(new_pending_files[0]));
		if (heap->files != NULL)
			free(heap->files);
		heap->files = new_pending_files;
		heap->allocated = new_size;
	}

	file_key = file->key = key;

	/*
	 * Start with hole at end, walk it up tree to find insertion point.
	 */
	hole = heap->used++;
	while (hole > 0) {
		parent = (hole - 1)/2;
		parent_key = heap->files[parent]->key;
		if (file_key >= parent_key) {
			heap->files[hole] = file;
			return (ARCHIVE_OK);
		}
		/* Move parent into hole <==> move hole up tree. */
		heap->files[hole] = heap->files[parent];
		hole = parent;
	}
	heap->files[0] = file;

	return (ARCHIVE_OK);
}
