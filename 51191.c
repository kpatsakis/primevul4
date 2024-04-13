_archive_read_next_header(struct archive *_a, struct archive_entry **entryp)
{
	int ret;
	struct archive_read *a = (struct archive_read *)_a;
	*entryp = NULL;
	ret = _archive_read_next_header2(_a, a->entry);
	*entryp = a->entry;
	return ret;
}
