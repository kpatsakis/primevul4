_archive_read_free(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	struct archive_read_passphrase *p;
	int i, n;
	int slots;
	int r = ARCHIVE_OK;

	if (_a == NULL)
		return (ARCHIVE_OK);
	archive_check_magic(_a, ARCHIVE_READ_MAGIC,
	    ARCHIVE_STATE_ANY | ARCHIVE_STATE_FATAL, "archive_read_free");
	if (a->archive.state != ARCHIVE_STATE_CLOSED
	    && a->archive.state != ARCHIVE_STATE_FATAL)
		r = archive_read_close(&a->archive);

	/* Call cleanup functions registered by optional components. */
	if (a->cleanup_archive_extract != NULL)
		r = (a->cleanup_archive_extract)(a);

	/* Cleanup format-specific data. */
	slots = sizeof(a->formats) / sizeof(a->formats[0]);
	for (i = 0; i < slots; i++) {
		a->format = &(a->formats[i]);
		if (a->formats[i].cleanup)
			(a->formats[i].cleanup)(a);
	}

	/* Free the filters */
	__archive_read_free_filters(a);

	/* Release the bidder objects. */
	n = sizeof(a->bidders)/sizeof(a->bidders[0]);
	for (i = 0; i < n; i++) {
		if (a->bidders[i].free != NULL) {
			int r1 = (a->bidders[i].free)(&a->bidders[i]);
			if (r1 < r)
				r = r1;
		}
	}

	/* Release passphrase list. */
	p = a->passphrases.first;
	while (p != NULL) {
		struct archive_read_passphrase *np = p->next;

		/* A passphrase should be cleaned. */
		memset(p->passphrase, 0, strlen(p->passphrase));
		free(p->passphrase);
		free(p);
		p = np;
	}

	archive_string_free(&a->archive.error_string);
	if (a->entry)
		archive_entry_free(a->entry);
	a->archive.magic = 0;
	__archive_clean(&a->archive);
	free(a->client.dataset);
	free(a);
	return (r);
}
