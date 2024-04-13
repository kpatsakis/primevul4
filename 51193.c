advance_file_pointer(struct archive_read_filter *filter, int64_t request)
{
	int64_t bytes_skipped, total_bytes_skipped = 0;
	ssize_t bytes_read;
	size_t min;

	if (filter->fatal)
		return (-1);

	/* Use up the copy buffer first. */
	if (filter->avail > 0) {
		min = (size_t)minimum(request, (int64_t)filter->avail);
		filter->next += min;
		filter->avail -= min;
		request -= min;
		filter->position += min;
		total_bytes_skipped += min;
	}

	/* Then use up the client buffer. */
	if (filter->client_avail > 0) {
		min = (size_t)minimum(request, (int64_t)filter->client_avail);
		filter->client_next += min;
		filter->client_avail -= min;
		request -= min;
		filter->position += min;
		total_bytes_skipped += min;
	}
	if (request == 0)
		return (total_bytes_skipped);

	/* If there's an optimized skip function, use it. */
	if (filter->skip != NULL) {
		bytes_skipped = (filter->skip)(filter, request);
		if (bytes_skipped < 0) {	/* error */
			filter->fatal = 1;
			return (bytes_skipped);
		}
		filter->position += bytes_skipped;
		total_bytes_skipped += bytes_skipped;
		request -= bytes_skipped;
		if (request == 0)
			return (total_bytes_skipped);
	}

	/* Use ordinary reads as necessary to complete the request. */
	for (;;) {
		bytes_read = (filter->read)(filter, &filter->client_buff);
		if (bytes_read < 0) {
			filter->client_buff = NULL;
			filter->fatal = 1;
			return (bytes_read);
		}

		if (bytes_read == 0) {
			if (filter->archive->client.cursor !=
			      filter->archive->client.nodes - 1) {
				if (client_switch_proxy(filter,
				    filter->archive->client.cursor + 1)
				    == ARCHIVE_OK)
					continue;
			}
			filter->client_buff = NULL;
			filter->end_of_file = 1;
			return (total_bytes_skipped);
		}

		if (bytes_read >= request) {
			filter->client_next =
			    ((const char *)filter->client_buff) + request;
			filter->client_avail = (size_t)(bytes_read - request);
			filter->client_total = bytes_read;
			total_bytes_skipped += request;
			filter->position += request;
			return (total_bytes_skipped);
		}

		filter->position += bytes_read;
		total_bytes_skipped += bytes_read;
		request -= bytes_read;
	}
}
