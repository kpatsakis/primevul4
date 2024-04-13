lha_read_file_header_3(struct archive_read *a, struct lha *lha)
{
	const unsigned char *p;
	size_t extdsize;
	int err;
	uint16_t header_crc;

	if ((p = __archive_read_ahead(a, H3_FIXED_SIZE, NULL)) == NULL)
		return (truncated_error(a));

	if (archive_le16dec(p + H3_FIELD_LEN_OFFSET) != 4)
		goto invalid;
	lha->header_size =archive_le32dec(p + H3_HEADER_SIZE_OFFSET);
	lha->compsize = archive_le32dec(p + H3_COMP_SIZE_OFFSET);
	lha->origsize = archive_le32dec(p + H3_ORIG_SIZE_OFFSET);
	lha->mtime = archive_le32dec(p + H3_TIME_OFFSET);
	lha->crc = archive_le16dec(p + H3_CRC_OFFSET);
	lha->setflag |= CRC_IS_SET;

	if (lha->header_size < H3_FIXED_SIZE + 4)
		goto invalid;
	header_crc = lha_crc16(0, p, H3_FIXED_SIZE);
	__archive_read_consume(a, H3_FIXED_SIZE);

	/* Read extended headers */
	err = lha_read_file_extended_header(a, lha, &header_crc, 4,
		  lha->header_size - H3_FIXED_SIZE, &extdsize);
	if (err < ARCHIVE_WARN)
		return (err);

	if (header_crc != lha->header_crc) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "LHa header CRC error");
		return (ARCHIVE_FATAL);
	}
	return (err);
invalid:
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "Invalid LHa header");
	return (ARCHIVE_FATAL);
}
