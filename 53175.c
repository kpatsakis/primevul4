archive_read_format_zip_streamable_read_header(struct archive_read *a,
    struct archive_entry *entry)
{
	struct zip *zip;

	a->archive.archive_format = ARCHIVE_FORMAT_ZIP;
	if (a->archive.archive_format_name == NULL)
		a->archive.archive_format_name = "ZIP";

	zip = (struct zip *)(a->format->data);

	/*
	 * It should be sufficient to call archive_read_next_header() for
	 * a reader to determine if an entry is encrypted or not. If the
	 * encryption of an entry is only detectable when calling
	 * archive_read_data(), so be it. We'll do the same check there
	 * as well.
	 */
	if (zip->has_encrypted_entries ==
			ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW)
		zip->has_encrypted_entries = 0;

	/* Make sure we have a zip_entry structure to use. */
	if (zip->zip_entries == NULL) {
		zip->zip_entries = malloc(sizeof(struct zip_entry));
		if (zip->zip_entries == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "Out  of memory");
			return ARCHIVE_FATAL;
		}
	}
	zip->entry = zip->zip_entries;
	memset(zip->entry, 0, sizeof(struct zip_entry));

	if (zip->cctx_valid)
		archive_decrypto_aes_ctr_release(&zip->cctx);
	if (zip->hctx_valid)
		archive_hmac_sha1_cleanup(&zip->hctx);
	zip->tctx_valid = zip->cctx_valid = zip->hctx_valid = 0;
	__archive_read_reset_passphrase(a);

	/* Search ahead for the next local file header. */
	__archive_read_consume(a, zip->unconsumed);
	zip->unconsumed = 0;
	for (;;) {
		int64_t skipped = 0;
		const char *p, *end;
		ssize_t bytes;

		p = __archive_read_ahead(a, 4, &bytes);
		if (p == NULL)
			return (ARCHIVE_FATAL);
		end = p + bytes;

		while (p + 4 <= end) {
			if (p[0] == 'P' && p[1] == 'K') {
				if (p[2] == '\003' && p[3] == '\004') {
					/* Regular file entry. */
					__archive_read_consume(a, skipped);
					return zip_read_local_file_header(a,
					    entry, zip);
				}

                              /*
                               * TODO: We cannot restore permissions
                               * based only on the local file headers.
                               * Consider scanning the central
                               * directory and returning additional
                               * entries for at least directories.
                               * This would allow us to properly set
                               * directory permissions.
			       *
			       * This won't help us fix symlinks
			       * and may not help with regular file
			       * permissions, either.  <sigh>
                               */
                              if (p[2] == '\001' && p[3] == '\002') {
                                      return (ARCHIVE_EOF);
                              }

                              /* End of central directory?  Must be an
                               * empty archive. */
                              if ((p[2] == '\005' && p[3] == '\006')
                                  || (p[2] == '\006' && p[3] == '\006'))
                                      return (ARCHIVE_EOF);
			}
			++p;
			++skipped;
		}
		__archive_read_consume(a, skipped);
	}
}
