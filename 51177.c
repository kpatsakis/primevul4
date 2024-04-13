__archive_read_close_filters(struct archive_read *a)
{
	struct archive_read_filter *f = a->filter;
	int r = ARCHIVE_OK;
	/* Close each filter in the pipeline. */
	while (f != NULL) {
		struct archive_read_filter *t = f->upstream;
		if (!f->closed && f->close != NULL) {
			int r1 = (f->close)(f);
			f->closed = 1;
			if (r1 < r)
				r = r1;
		}
		free(f->buffer);
		f->buffer = NULL;
		f = t;
	}
	return r;
}
