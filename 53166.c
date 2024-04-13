archive_read_format_zip_cleanup(struct archive_read *a)
{
	struct zip *zip;
	struct zip_entry *zip_entry, *next_zip_entry;

	zip = (struct zip *)(a->format->data);
#ifdef HAVE_ZLIB_H
	if (zip->stream_valid)
		inflateEnd(&zip->stream);
	free(zip->uncompressed_buffer);
#endif
	if (zip->zip_entries) {
		zip_entry = zip->zip_entries;
		while (zip_entry != NULL) {
			next_zip_entry = zip_entry->next;
			archive_string_free(&zip_entry->rsrcname);
			free(zip_entry);
			zip_entry = next_zip_entry;
		}
	}
	free(zip->decrypted_buffer);
	if (zip->cctx_valid)
		archive_decrypto_aes_ctr_release(&zip->cctx);
	if (zip->hctx_valid)
		archive_hmac_sha1_cleanup(&zip->hctx);
	free(zip->iv);
	free(zip->erd);
	free(zip->v_data);
	archive_string_free(&zip->format_name);
	free(zip);
	(a->format->data) = NULL;
	return (ARCHIVE_OK);
}
