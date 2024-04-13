read_Times(struct archive_read *a, struct _7z_header_info *h, int type)
{
	struct _7zip *zip = (struct _7zip *)a->format->data;
	const unsigned char *p;
	struct _7zip_entry *entries = zip->entries;
	unsigned char *timeBools;
	int allAreDefined;
	unsigned i;

	timeBools = calloc((size_t)zip->numFiles, sizeof(*timeBools));
	if (timeBools == NULL)
		return (-1);

	/* Read allAreDefined. */
	if ((p = header_bytes(a, 1)) == NULL)
		goto failed;
	allAreDefined = *p;
	if (allAreDefined)
		memset(timeBools, 1, (size_t)zip->numFiles);
	else {
		if (read_Bools(a, timeBools, (size_t)zip->numFiles) < 0)
			goto failed;
	}

	/* Read external. */
	if ((p = header_bytes(a, 1)) == NULL)
		goto failed;
	if (*p) {
		if (parse_7zip_uint64(a, &(h->dataIndex)) < 0)
			goto failed;
		if (UMAX_ENTRY < h->dataIndex)
			goto failed;
	}

	for (i = 0; i < zip->numFiles; i++) {
		if (!timeBools[i])
			continue;
		if ((p = header_bytes(a, 8)) == NULL)
			goto failed;
		switch (type) {
		case kCTime:
			fileTimeToUtc(archive_le64dec(p),
			    &(entries[i].ctime),
			    &(entries[i].ctime_ns));
			entries[i].flg |= CTIME_IS_SET;
			break;
		case kATime:
			fileTimeToUtc(archive_le64dec(p),
			    &(entries[i].atime),
			    &(entries[i].atime_ns));
			entries[i].flg |= ATIME_IS_SET;
			break;
		case kMTime:
			fileTimeToUtc(archive_le64dec(p),
			    &(entries[i].mtime),
			    &(entries[i].mtime_ns));
			entries[i].flg |= MTIME_IS_SET;
			break;
		}
	}

	free(timeBools);
	return (0);
failed:
	free(timeBools);
	return (-1);
}
