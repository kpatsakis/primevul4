__archive_write_open_filter(struct archive_write_filter *f)
{
	if (f->open == NULL)
		return (ARCHIVE_OK);
	return (f->open)(f);
}
