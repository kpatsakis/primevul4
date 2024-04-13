read_CodersInfo(struct archive_read *a, struct _7z_coders_info *ci)
{
	const unsigned char *p;
	struct _7z_digests digest;
	unsigned i;

	memset(ci, 0, sizeof(*ci));
	memset(&digest, 0, sizeof(digest));

	if ((p = header_bytes(a, 1)) == NULL)
		goto failed;
	if (*p != kFolder)
		goto failed;

	/*
	 * Read NumFolders.
	 */
	if (parse_7zip_uint64(a, &(ci->numFolders)) < 0)
		goto failed;
	if (UMAX_ENTRY < ci->numFolders)
		return (-1);

	/*
	 * Read External.
	 */
	if ((p = header_bytes(a, 1)) == NULL)
		goto failed;
	switch (*p) {
	case 0:
		ci->folders =
			calloc((size_t)ci->numFolders, sizeof(*ci->folders));
		if (ci->folders == NULL)
			return (-1);
		for (i = 0; i < ci->numFolders; i++) {
			if (read_Folder(a, &(ci->folders[i])) < 0)
				goto failed;
		}
		break;
	case 1:
		if (parse_7zip_uint64(a, &(ci->dataStreamIndex)) < 0)
			return (-1);
		if (UMAX_ENTRY < ci->dataStreamIndex)
			return (-1);
		if (ci->numFolders > 0) {
			archive_set_error(&a->archive, -1,
			    "Malformed 7-Zip archive");
			goto failed;
		}
		break;
	default:
		archive_set_error(&a->archive, -1,
		    "Malformed 7-Zip archive");
		goto failed;
	}

	if ((p = header_bytes(a, 1)) == NULL)
		goto failed;
	if (*p != kCodersUnPackSize)
		goto failed;

	for (i = 0; i < ci->numFolders; i++) {
		struct _7z_folder *folder = &(ci->folders[i]);
		unsigned j;

		folder->unPackSize =
		    calloc((size_t)folder->numOutStreams, sizeof(*folder->unPackSize));
		if (folder->unPackSize == NULL)
			goto failed;
		for (j = 0; j < folder->numOutStreams; j++) {
			if (parse_7zip_uint64(a, &(folder->unPackSize[j])) < 0)
				goto failed;
		}
	}

	/*
	 * Read CRCs.
	 */
	if ((p = header_bytes(a, 1)) == NULL)
		goto failed;
	if (*p == kEnd)
		return (0);
	if (*p != kCRC)
		goto failed;
	if (read_Digests(a, &digest, (size_t)ci->numFolders) < 0)
		goto failed;
	for (i = 0; i < ci->numFolders; i++) {
		ci->folders[i].digest_defined = digest.defineds[i];
		ci->folders[i].digest = digest.digests[i];
	}

	/*
	 *  Must be kEnd.
	 */
	if ((p = header_bytes(a, 1)) == NULL)
		goto failed;
	if (*p != kEnd)
		goto failed;
	free_Digest(&digest);
	return (0);
failed:
	free_Digest(&digest);
	return (-1);
}
