lha_end_of_entry(struct archive_read *a)
{
	struct lha *lha = (struct lha *)(a->format->data);
	int r = ARCHIVE_EOF;

	if (!lha->end_of_entry_cleanup) {
		if ((lha->setflag & CRC_IS_SET) &&
		    lha->crc != lha->entry_crc_calculated) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "LHa data CRC error");
			r = ARCHIVE_WARN;
		}

		/* End-of-entry cleanup done. */
		lha->end_of_entry_cleanup = 1;
	}
	return (r);
}
