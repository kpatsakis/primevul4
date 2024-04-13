register_CE(struct archive_read *a, int32_t location,
    struct file_info *file)
{
	struct iso9660 *iso9660;
	struct read_ce_queue *heap;
	struct read_ce_req *p;
	uint64_t offset, parent_offset;
	int hole, parent;

	iso9660 = (struct iso9660 *)(a->format->data);
	offset = ((uint64_t)location) * (uint64_t)iso9660->logical_block_size;
	if (((file->mode & AE_IFMT) == AE_IFREG &&
	    offset >= file->offset) ||
	    offset < iso9660->current_position ||
	    (((uint64_t)file->ce_offset) + file->ce_size)
	      > (uint64_t)iso9660->logical_block_size ||
	    offset + file->ce_offset + file->ce_size
		  > iso9660->volume_size) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Invalid parameter in SUSP \"CE\" extension");
		return (ARCHIVE_FATAL);
	}

	/* Expand our CE list as necessary. */
	heap = &(iso9660->read_ce_req);
	if (heap->cnt >= heap->allocated) {
		int new_size;

		if (heap->allocated < 16)
			new_size = 16;
		else
			new_size = heap->allocated * 2;
		/* Overflow might keep us from growing the list. */
		if (new_size <= heap->allocated) {
			archive_set_error(&a->archive, ENOMEM, "Out of memory");
			return (ARCHIVE_FATAL);
		}
		p = calloc(new_size, sizeof(p[0]));
		if (p == NULL) {
			archive_set_error(&a->archive, ENOMEM, "Out of memory");
			return (ARCHIVE_FATAL);
		}
		if (heap->reqs != NULL) {
			memcpy(p, heap->reqs, heap->cnt * sizeof(*p));
			free(heap->reqs);
		}
		heap->reqs = p;
		heap->allocated = new_size;
	}

	/*
	 * Start with hole at end, walk it up tree to find insertion point.
	 */
	hole = heap->cnt++;
	while (hole > 0) {
		parent = (hole - 1)/2;
		parent_offset = heap->reqs[parent].offset;
		if (offset >= parent_offset) {
			heap->reqs[hole].offset = offset;
			heap->reqs[hole].file = file;
			return (ARCHIVE_OK);
		}
		/* Move parent into hole <==> move hole up tree. */
		heap->reqs[hole] = heap->reqs[parent];
		hole = parent;
	}
	heap->reqs[0].offset = offset;
	heap->reqs[0].file = file;
	return (ARCHIVE_OK);
}
