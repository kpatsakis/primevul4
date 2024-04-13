xar_read_data(struct archive_read *a,
    const void **buff, size_t *size, int64_t *offset)
{
	struct xar *xar;
	size_t used;
	int r;

	xar = (struct xar *)(a->format->data);

	if (xar->entry_unconsumed) {
		__archive_read_consume(a, xar->entry_unconsumed);
		xar->entry_unconsumed = 0;
	}

	if (xar->end_of_file || xar->entry_remaining <= 0) {
		r = ARCHIVE_EOF;
		goto abort_read_data;
	}

	if (xar->entry_init) {
		r = rd_contents_init(a, xar->entry_encoding,
		    xar->entry_a_sum.alg, xar->entry_e_sum.alg);
		if (r != ARCHIVE_OK) {
			xar->entry_remaining = 0;
			return (r);
		}
		xar->entry_init = 0;
	}

	*buff = NULL;
	r = rd_contents(a, buff, size, &used, xar->entry_remaining);
	if (r != ARCHIVE_OK)
		goto abort_read_data;

	*offset = xar->entry_total;
	xar->entry_total += *size;
	xar->total += *size;
	xar->offset += used;
	xar->entry_remaining -= used;
	xar->entry_unconsumed = used;

	if (xar->entry_remaining == 0) {
		if (xar->entry_total != xar->entry_size) {
			archive_set_error(&(a->archive), ARCHIVE_ERRNO_MISC,
			    "Decompressed size error");
			r = ARCHIVE_FATAL;
			goto abort_read_data;
		}
		r = checksum_final(a,
		    xar->entry_a_sum.val, xar->entry_a_sum.len,
		    xar->entry_e_sum.val, xar->entry_e_sum.len);
		if (r != ARCHIVE_OK)
			goto abort_read_data;
	}

	return (ARCHIVE_OK);
abort_read_data:
	*buff = NULL;
	*size = 0;
	*offset = xar->total;
	return (r);
}
