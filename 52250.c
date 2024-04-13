archive_read_format_7zip_cleanup(struct archive_read *a)
{
	struct _7zip *zip;

	zip = (struct _7zip *)(a->format->data);
	free_StreamsInfo(&(zip->si));
	free(zip->entries);
	free(zip->entry_names);
	free_decompression(a, zip);
	free(zip->uncompressed_buffer);
	free(zip->sub_stream_buff[0]);
	free(zip->sub_stream_buff[1]);
	free(zip->sub_stream_buff[2]);
	free(zip->tmp_stream_buff);
	free(zip);
	(a->format->data) = NULL;
	return (ARCHIVE_OK);
}
