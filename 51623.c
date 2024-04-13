zisofs_read_data(struct archive_read *a,
    const void **buff, size_t *size, int64_t *offset)
{

	(void)buff;/* UNUSED */
	(void)size;/* UNUSED */
	(void)offset;/* UNUSED */
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "zisofs is not supported on this platform.");
	return (ARCHIVE_FAILED);
}
