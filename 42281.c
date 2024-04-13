archive_write_client_open(struct archive_write_filter *f)
{
	struct archive_write *a = (struct archive_write *)f->archive;
	struct archive_none *state;
	void *buffer;
	size_t buffer_size;

	f->bytes_per_block = archive_write_get_bytes_per_block(f->archive);
	f->bytes_in_last_block =
	    archive_write_get_bytes_in_last_block(f->archive);
	buffer_size = f->bytes_per_block;

	state = (struct archive_none *)calloc(1, sizeof(*state));
	buffer = (char *)malloc(buffer_size);
	if (state == NULL || buffer == NULL) {
		free(state);
		free(buffer);
		archive_set_error(f->archive, ENOMEM,
		    "Can't allocate data for output buffering");
		return (ARCHIVE_FATAL);
	}

	state->buffer_size = buffer_size;
	state->buffer = buffer;
	state->next = state->buffer;
	state->avail = state->buffer_size;
	f->data = state;

	if (a->client_opener == NULL)
		return (ARCHIVE_OK);
	return (a->client_opener(f->archive, a->client_data));
}
