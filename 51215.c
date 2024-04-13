choose_format(struct archive_read *a)
{
	int slots;
	int i;
	int bid, best_bid;
	int best_bid_slot;

	slots = sizeof(a->formats) / sizeof(a->formats[0]);
	best_bid = -1;
	best_bid_slot = -1;

	/* Set up a->format for convenience of bidders. */
	a->format = &(a->formats[0]);
	for (i = 0; i < slots; i++, a->format++) {
		if (a->format->bid) {
			bid = (a->format->bid)(a, best_bid);
			if (bid == ARCHIVE_FATAL)
				return (ARCHIVE_FATAL);
			if (a->filter->position != 0)
				__archive_read_seek(a, 0, SEEK_SET);
			if ((bid > best_bid) || (best_bid_slot < 0)) {
				best_bid = bid;
				best_bid_slot = i;
			}
		}
	}

	/*
	 * There were no bidders; this is a serious programmer error
	 * and demands a quick and definitive abort.
	 */
	if (best_bid_slot < 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "No formats registered");
		return (ARCHIVE_FATAL);
	}

	/*
	 * There were bidders, but no non-zero bids; this means we
	 * can't support this stream.
	 */
	if (best_bid < 1) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Unrecognized archive format");
		return (ARCHIVE_FATAL);
	}

	return (best_bid_slot);
}
