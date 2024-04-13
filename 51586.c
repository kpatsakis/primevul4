archive_read_format_iso9660_cleanup(struct archive_read *a)
{
	struct iso9660 *iso9660;
	int r = ARCHIVE_OK;

	iso9660 = (struct iso9660 *)(a->format->data);
	release_files(iso9660);
	free(iso9660->read_ce_req.reqs);
	archive_string_free(&iso9660->pathname);
	archive_string_free(&iso9660->previous_pathname);
	if (iso9660->pending_files.files)
		free(iso9660->pending_files.files);
#ifdef HAVE_ZLIB_H
	free(iso9660->entry_zisofs.uncompressed_buffer);
	free(iso9660->entry_zisofs.block_pointers);
	if (iso9660->entry_zisofs.stream_valid) {
		if (inflateEnd(&iso9660->entry_zisofs.stream) != Z_OK) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Failed to clean up zlib decompressor");
			r = ARCHIVE_FATAL;
		}
	}
#endif
	free(iso9660->utf16be_path);
	free(iso9660->utf16be_previous_path);
	free(iso9660);
	(a->format->data) = NULL;
	return (r);
}
