read_CE(struct archive_read *a, struct iso9660 *iso9660)
{
	struct read_ce_queue *heap;
	const unsigned char *b, *p, *end;
	struct file_info *file;
	size_t step;
	int r;

	/* Read data which RRIP "CE" extension points. */
	heap = &(iso9660->read_ce_req);
	step = iso9660->logical_block_size;
	while (heap->cnt &&
	    heap->reqs[0].offset == iso9660->current_position) {
		b = __archive_read_ahead(a, step, NULL);
		if (b == NULL) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_MISC,
			    "Failed to read full block when scanning "
			    "ISO9660 directory list");
			return (ARCHIVE_FATAL);
		}
		do {
			file = heap->reqs[0].file;
			if (file->ce_offset + file->ce_size > step) {
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_FILE_FORMAT,
				    "Malformed CE information");
				return (ARCHIVE_FATAL);
			}
			p = b + file->ce_offset;
			end = p + file->ce_size;
			next_CE(heap);
			r = parse_rockridge(a, file, p, end);
			if (r != ARCHIVE_OK)
				return (ARCHIVE_FATAL);
		} while (heap->cnt &&
		    heap->reqs[0].offset == iso9660->current_position);
		/* NOTE: Do not move this consume's code to fron of
		 * do-while loop. Registration of nested CE extension
		 * might cause error because of current position. */
		__archive_read_consume(a, step);
		iso9660->current_position += step;
	}
	return (ARCHIVE_OK);
}
