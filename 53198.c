zip_alloc_decryption_buffer(struct archive_read *a)
{
	struct zip *zip = (struct zip *)(a->format->data);
	size_t bs = 256 * 1024;

	if (zip->decrypted_buffer == NULL) {
		zip->decrypted_buffer_size = bs;
		zip->decrypted_buffer = malloc(bs);
		if (zip->decrypted_buffer == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "No memory for ZIP decryption");
			return (ARCHIVE_FATAL);
		}
	}
	zip->decrypted_ptr = zip->decrypted_buffer;
	return (ARCHIVE_OK);
}
