read_toc(struct archive_read *a)
{
	struct xar *xar;
	struct xar_file *file;
	const unsigned char *b;
	uint64_t toc_compressed_size;
	uint64_t toc_uncompressed_size;
	uint32_t toc_chksum_alg;
	ssize_t bytes;
	int r;

	xar = (struct xar *)(a->format->data);

	/*
	 * Read xar header.
	 */
	b = __archive_read_ahead(a, HEADER_SIZE, &bytes);
	if (bytes < 0)
		return ((int)bytes);
	if (bytes < HEADER_SIZE) {
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated archive header");
		return (ARCHIVE_FATAL);
	}

	if (archive_be32dec(b) != HEADER_MAGIC) {
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Invalid header magic");
		return (ARCHIVE_FATAL);
	}
	if (archive_be16dec(b+6) != HEADER_VERSION) {
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Unsupported header version(%d)",
		    archive_be16dec(b+6));
		return (ARCHIVE_FATAL);
	}
	toc_compressed_size = archive_be64dec(b+8);
	xar->toc_remaining = toc_compressed_size;
	toc_uncompressed_size = archive_be64dec(b+16);
	toc_chksum_alg = archive_be32dec(b+24);
	__archive_read_consume(a, HEADER_SIZE);
	xar->offset += HEADER_SIZE;
	xar->toc_total = 0;

	/*
	 * Read TOC(Table of Contents).
	 */
	/* Initialize reading contents. */
	r = move_reading_point(a, HEADER_SIZE);
	if (r != ARCHIVE_OK)
		return (r);
	r = rd_contents_init(a, GZIP, toc_chksum_alg, CKSUM_NONE);
	if (r != ARCHIVE_OK)
		return (r);

#ifdef HAVE_LIBXML_XMLREADER_H
	r = xml2_read_toc(a);
#elif defined(HAVE_BSDXML_H) || defined(HAVE_EXPAT_H)
	r = expat_read_toc(a);
#endif
	if (r != ARCHIVE_OK)
		return (r);

	/* Set 'The HEAP' base. */
	xar->h_base = xar->offset;
	if (xar->toc_total != toc_uncompressed_size) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "TOC uncompressed size error");
		return (ARCHIVE_FATAL);
	}

	/*
	 * Checksum TOC
	 */
	if (toc_chksum_alg != CKSUM_NONE) {
		r = move_reading_point(a, xar->toc_chksum_offset);
		if (r != ARCHIVE_OK)
			return (r);
		b = __archive_read_ahead(a,
			(size_t)xar->toc_chksum_size, &bytes);
		if (bytes < 0)
			return ((int)bytes);
		if ((uint64_t)bytes < xar->toc_chksum_size) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Truncated archive file");
			return (ARCHIVE_FATAL);
		}
		r = checksum_final(a, b,
			(size_t)xar->toc_chksum_size, NULL, 0);
		__archive_read_consume(a, xar->toc_chksum_size);
		xar->offset += xar->toc_chksum_size;
		if (r != ARCHIVE_OK)
			return (ARCHIVE_FATAL);
	}

	/*
	 * Connect hardlinked files.
	 */
	for (file = xar->hdlink_orgs; file != NULL; file = file->hdnext) {
		struct hdlink **hdlink;

		for (hdlink = &(xar->hdlink_list); *hdlink != NULL;
		    hdlink = &((*hdlink)->next)) {
			if ((*hdlink)->id == file->id) {
				struct hdlink *hltmp;
				struct xar_file *f2;
				int nlink = (*hdlink)->cnt + 1;

				file->nlink = nlink;
				for (f2 = (*hdlink)->files; f2 != NULL;
				    f2 = f2->hdnext) {
					f2->nlink = nlink;
					archive_string_copy(
					    &(f2->hardlink), &(file->pathname));
				}
				/* Remove resolved files from hdlist_list. */
				hltmp = *hdlink;
				*hdlink = hltmp->next;
				free(hltmp);
				break;
			}
		}
	}
	a->archive.archive_format = ARCHIVE_FORMAT_XAR;
	a->archive.archive_format_name = "xar";

	return (ARCHIVE_OK);
}
