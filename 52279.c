read_StreamsInfo(struct archive_read *a, struct _7z_stream_info *si)
{
	struct _7zip *zip = (struct _7zip *)a->format->data;
	const unsigned char *p;
	unsigned i;

	memset(si, 0, sizeof(*si));

	if ((p = header_bytes(a, 1)) == NULL)
		return (-1);
	if (*p == kPackInfo) {
		uint64_t packPos;

		if (read_PackInfo(a, &(si->pi)) < 0)
			return (-1);

		if (si->pi.positions == NULL || si->pi.sizes == NULL)
			return (-1);
		/*
		 * Calculate packed stream positions.
		 */
		packPos = si->pi.pos;
		for (i = 0; i < si->pi.numPackStreams; i++) {
			si->pi.positions[i] = packPos;
			packPos += si->pi.sizes[i];
			if (packPos > zip->header_offset)
				return (-1);
		}
		if ((p = header_bytes(a, 1)) == NULL)
			return (-1);
	}
	if (*p == kUnPackInfo) {
		uint32_t packIndex;
		struct _7z_folder *f;

		if (read_CodersInfo(a, &(si->ci)) < 0)
			return (-1);

		/*
		 * Calculate packed stream indexes.
		 */
		packIndex = 0;
		f = si->ci.folders;
		for (i = 0; i < si->ci.numFolders; i++) {
			f[i].packIndex = packIndex;
			packIndex += (uint32_t)f[i].numPackedStreams;
			if (packIndex > si->pi.numPackStreams)
				return (-1);
		}
		if ((p = header_bytes(a, 1)) == NULL)
			return (-1);
	}

	if (*p == kSubStreamsInfo) {
		if (read_SubStreamsInfo(a, &(si->ss),
		    si->ci.folders, (size_t)si->ci.numFolders) < 0)
			return (-1);
		if ((p = header_bytes(a, 1)) == NULL)
			return (-1);
	}

	/*
	 *  Must be kEnd.
	 */
	if (*p != kEnd)
		return (-1);
	return (0);
}
