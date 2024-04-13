decode_encoded_header_info(struct archive_read *a, struct _7z_stream_info *si)
{
	struct _7zip *zip = (struct _7zip *)a->format->data;

	errno = 0;
	if (read_StreamsInfo(a, si) < 0) {
		if (errno == ENOMEM)
			archive_set_error(&a->archive, -1,
			    "Couldn't allocate memory");
		else
			archive_set_error(&a->archive, -1,
			    "Malformed 7-Zip archive");
		return (ARCHIVE_FATAL);
	}

	if (si->pi.numPackStreams == 0 || si->ci.numFolders == 0) {
		archive_set_error(&a->archive, -1, "Malformed 7-Zip archive");
		return (ARCHIVE_FATAL);
	}

	if (zip->header_offset < si->pi.pos + si->pi.sizes[0] ||
	    (int64_t)(si->pi.pos + si->pi.sizes[0]) < 0 ||
	    si->pi.sizes[0] == 0 || (int64_t)si->pi.pos < 0) {
		archive_set_error(&a->archive, -1, "Malformed Header offset");
		return (ARCHIVE_FATAL);
	}

	return (ARCHIVE_OK);
}
