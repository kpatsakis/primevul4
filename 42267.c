__archive_write_close_filter(struct archive_write_filter *f)
{
	if (f->close != NULL)
		return (f->close)(f);
	if (f->next_filter != NULL)
		return (__archive_write_close_filter(f->next_filter));
	return (ARCHIVE_OK);
}
