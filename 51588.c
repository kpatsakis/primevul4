archive_read_format_iso9660_read_data(struct archive_read *a,
    const void **buff, size_t *size, int64_t *offset)
{
	ssize_t bytes_read;
	struct iso9660 *iso9660;

	iso9660 = (struct iso9660 *)(a->format->data);

	if (iso9660->entry_bytes_unconsumed) {
		__archive_read_consume(a, iso9660->entry_bytes_unconsumed);
		iso9660->entry_bytes_unconsumed = 0;
	}

	if (iso9660->entry_bytes_remaining <= 0) {
		if (iso9660->entry_content != NULL)
			iso9660->entry_content = iso9660->entry_content->next;
		if (iso9660->entry_content == NULL) {
			*buff = NULL;
			*size = 0;
			*offset = iso9660->entry_sparse_offset;
			return (ARCHIVE_EOF);
		}
		/* Seek forward to the start of the entry. */
		if (iso9660->current_position < iso9660->entry_content->offset) {
			int64_t step;

			step = iso9660->entry_content->offset -
			    iso9660->current_position;
			step = __archive_read_consume(a, step);
			if (step < 0)
				return ((int)step);
			iso9660->current_position =
			    iso9660->entry_content->offset;
		}
		if (iso9660->entry_content->offset < iso9660->current_position) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Ignoring out-of-order file (%s) %jd < %jd",
			    iso9660->pathname.s,
			    (intmax_t)iso9660->entry_content->offset,
			    (intmax_t)iso9660->current_position);
			*buff = NULL;
			*size = 0;
			*offset = iso9660->entry_sparse_offset;
			return (ARCHIVE_WARN);
		}
		iso9660->entry_bytes_remaining = iso9660->entry_content->size;
	}
	if (iso9660->entry_zisofs.pz)
		return (zisofs_read_data(a, buff, size, offset));

	*buff = __archive_read_ahead(a, 1, &bytes_read);
	if (bytes_read == 0)
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Truncated input file");
	if (*buff == NULL)
		return (ARCHIVE_FATAL);
	if (bytes_read > iso9660->entry_bytes_remaining)
		bytes_read = (ssize_t)iso9660->entry_bytes_remaining;
	*size = bytes_read;
	*offset = iso9660->entry_sparse_offset;
	iso9660->entry_sparse_offset += bytes_read;
	iso9660->entry_bytes_remaining -= bytes_read;
	iso9660->entry_bytes_unconsumed = bytes_read;
	iso9660->current_position += bytes_read;
	return (ARCHIVE_OK);
}
