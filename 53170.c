archive_read_format_zip_read_data_skip_seekable(struct archive_read *a)
{
	struct zip *zip;
	zip = (struct zip *)(a->format->data);

	zip->unconsumed = 0;
	return (ARCHIVE_OK);
}
