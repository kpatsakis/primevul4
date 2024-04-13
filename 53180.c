check_authentication_code(struct archive_read *a, const void *_p)
{
	struct zip *zip = (struct zip *)(a->format->data);

	/* Check authentication code. */
	if (zip->hctx_valid) {
		const void *p;
		uint8_t hmac[20];
		size_t hmac_len = 20;
		int cmp;

		archive_hmac_sha1_final(&zip->hctx, hmac, &hmac_len);
		if (_p == NULL) {
			/* Read authentication code. */
			p = __archive_read_ahead(a, AUTH_CODE_SIZE, NULL);
			if (p == NULL) {
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_FILE_FORMAT,
				    "Truncated ZIP file data");
				return (ARCHIVE_FATAL);
			}
		} else {
			p = _p;
		}
		cmp = memcmp(hmac, p, AUTH_CODE_SIZE);
		__archive_read_consume(a, AUTH_CODE_SIZE);
		if (cmp != 0) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_MISC,
			    "ZIP bad Authentication code");
			return (ARCHIVE_WARN);
		}
	}
	return (ARCHIVE_OK);
}
