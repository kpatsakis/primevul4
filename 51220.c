client_skip_proxy(struct archive_read_filter *self, int64_t request)
{
	if (request < 0)
		__archive_errx(1, "Negative skip requested.");
	if (request == 0)
		return 0;

	if (self->archive->client.skipper != NULL) {
		/* Seek requests over 1GiB are broken down into
		 * multiple seeks.  This avoids overflows when the
		 * requests get passed through 32-bit arguments. */
		int64_t skip_limit = (int64_t)1 << 30;
		int64_t total = 0;
		for (;;) {
			int64_t get, ask = request;
			if (ask > skip_limit)
				ask = skip_limit;
			get = (self->archive->client.skipper)
				(&self->archive->archive, self->data, ask);
			if (get == 0)
				return (total);
			request -= get;
			total += get;
		}
	} else if (self->archive->client.seeker != NULL
		&& request > 64 * 1024) {
		/* If the client provided a seeker but not a skipper,
		 * we can use the seeker to skip forward.
		 *
		 * Note: This isn't always a good idea.  The client
		 * skipper is allowed to skip by less than requested
		 * if it needs to maintain block alignment.  The
		 * seeker is not allowed to play such games, so using
		 * the seeker here may be a performance loss compared
		 * to just reading and discarding.  That's why we
		 * only do this for skips of over 64k.
		 */
		int64_t before = self->position;
		int64_t after = (self->archive->client.seeker)
		    (&self->archive->archive, self->data, request, SEEK_CUR);
		if (after != before + request)
			return ARCHIVE_FATAL;
		return after - before;
	}
	return 0;
}
