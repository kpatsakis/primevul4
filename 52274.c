ppmd_read(void *p)
{
	struct archive_read *a = ((IByteIn*)p)->a;
	struct _7zip *zip = (struct _7zip *)(a->format->data);
	Byte b;

	if (zip->ppstream.avail_in == 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated RAR file data");
		zip->ppstream.overconsumed = 1;
		return (0);
	}
	b = *zip->ppstream.next_in++;
	zip->ppstream.avail_in--;
	zip->ppstream.total_in++;
	return (b);
}
