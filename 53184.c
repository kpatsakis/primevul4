init_WinZip_AES_decryption(struct archive_read *a)
{
	struct zip *zip = (struct zip *)(a->format->data);
	const void *p;
	const uint8_t *pv;
	size_t key_len, salt_len;
	uint8_t derived_key[MAX_DERIVED_KEY_BUF_SIZE];
	int retry;
	int r;

	if (zip->cctx_valid || zip->hctx_valid)
		return (ARCHIVE_OK);

	switch (zip->entry->aes_extra.strength) {
	case 1: salt_len = 8;  key_len = 16; break;
	case 2: salt_len = 12; key_len = 24; break;
	case 3: salt_len = 16; key_len = 32; break;
	default: goto corrupted;
	}
	p = __archive_read_ahead(a, salt_len + 2, NULL);
	if (p == NULL)
		goto truncated;

	for (retry = 0;; retry++) {
		const char *passphrase;

		passphrase = __archive_read_next_passphrase(a);
		if (passphrase == NULL) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    (retry > 0)?
				"Incorrect passphrase":
				"Passphrase required for this entry");
			return (ARCHIVE_FAILED);
		}
		memset(derived_key, 0, sizeof(derived_key));
		r = archive_pbkdf2_sha1(passphrase, strlen(passphrase),
		    p, salt_len, 1000, derived_key, key_len * 2 + 2);
		if (r != 0) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Decryption is unsupported due to lack of "
			    "crypto library");
			return (ARCHIVE_FAILED);
		}

		/* Check password verification value. */
		pv = ((const uint8_t *)p) + salt_len;
		if (derived_key[key_len * 2] == pv[0] &&
		    derived_key[key_len * 2 + 1] == pv[1])
			break;/* The passphrase is OK. */
		if (retry > 10000) {
			/* Avoid infinity loop. */
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Too many incorrect passphrases");
			return (ARCHIVE_FAILED);
		}
	}

	r = archive_decrypto_aes_ctr_init(&zip->cctx, derived_key, key_len);
	if (r != 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Decryption is unsupported due to lack of crypto library");
		return (ARCHIVE_FAILED);
	}
	r = archive_hmac_sha1_init(&zip->hctx, derived_key + key_len, key_len);
	if (r != 0) {
		archive_decrypto_aes_ctr_release(&zip->cctx);
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "Failed to initialize HMAC-SHA1");
		return (ARCHIVE_FAILED);
	}
	zip->cctx_valid = zip->hctx_valid = 1;
	__archive_read_consume(a, salt_len + 2);
	zip->entry_bytes_remaining -= salt_len + 2 + AUTH_CODE_SIZE;
	if (0 == (zip->entry->zip_flags & ZIP_LENGTH_AT_END)
	    && zip->entry_bytes_remaining < 0)
		goto corrupted;
	zip->entry_compressed_bytes_read += salt_len + 2 + AUTH_CODE_SIZE;
	zip->decrypted_bytes_remaining = 0;

	zip->entry->compression = zip->entry->aes_extra.compression;
	return (zip_alloc_decryption_buffer(a));

truncated:
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "Truncated ZIP file data");
	return (ARCHIVE_FATAL);
corrupted:
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "Corrupted ZIP file data");
	return (ARCHIVE_FATAL);
}
