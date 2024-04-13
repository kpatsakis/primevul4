read_decryption_header(struct archive_read *a)
{
	struct zip *zip = (struct zip *)(a->format->data);
	const char *p;
	unsigned int remaining_size;
	unsigned int ts;

	/*
	 * Read an initialization vector data field.
	 */
	p = __archive_read_ahead(a, 2, NULL);
	if (p == NULL)
		goto truncated;
	ts = zip->iv_size;
	zip->iv_size = archive_le16dec(p);
	__archive_read_consume(a, 2);
	if (ts < zip->iv_size) {
		free(zip->iv);
		zip->iv = NULL;
	}
	p = __archive_read_ahead(a, zip->iv_size, NULL);
	if (p == NULL)
		goto truncated;
	if (zip->iv == NULL) {
		zip->iv = malloc(zip->iv_size);
		if (zip->iv == NULL)
			goto nomem;
	}
	memcpy(zip->iv, p, zip->iv_size);
	__archive_read_consume(a, zip->iv_size);

	/*
	 * Read a size of remaining decryption header field.
	 */
	p = __archive_read_ahead(a, 14, NULL);
	if (p == NULL)
		goto truncated;
	remaining_size = archive_le32dec(p);
	if (remaining_size < 16 || remaining_size > (1 << 18))
		goto corrupted;

	/* Check if format version is supported. */
	if (archive_le16dec(p+4) != 3) {
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Unsupported encryption format version: %u",
		    archive_le16dec(p+4));
		return (ARCHIVE_FAILED);
	}

	/*
	 * Read an encryption algorithm field.
	 */
	zip->alg_id = archive_le16dec(p+6);
	switch (zip->alg_id) {
	case 0x6601:/* DES */
	case 0x6602:/* RC2 */
	case 0x6603:/* 3DES 168 */
	case 0x6609:/* 3DES 112 */
	case 0x660E:/* AES 128 */
	case 0x660F:/* AES 192 */
	case 0x6610:/* AES 256 */
	case 0x6702:/* RC2 (version >= 5.2) */
	case 0x6720:/* Blowfish */
	case 0x6721:/* Twofish */
	case 0x6801:/* RC4 */
		/* Suuported encryption algorithm. */
		break;
	default:
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Unknown encryption algorithm: %u", zip->alg_id);
		return (ARCHIVE_FAILED);
	}

	/*
	 * Read a bit length field.
	 */
	zip->bit_len = archive_le16dec(p+8);

	/*
	 * Read a flags field.
	 */
	zip->flags = archive_le16dec(p+10);
	switch (zip->flags & 0xf000) {
	case 0x0001: /* Password is required to decrypt. */
	case 0x0002: /* Certificates only. */
	case 0x0003: /* Password or certificate required to decrypt. */
		break;
	default:
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Unknown encryption flag: %u", zip->flags);
		return (ARCHIVE_FAILED);
	}
	if ((zip->flags & 0xf000) == 0 ||
	    (zip->flags & 0xf000) == 0x4000) {
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_FILE_FORMAT,
		    "Unknown encryption flag: %u", zip->flags);
		return (ARCHIVE_FAILED);
	}

	/*
	 * Read an encrypted random data field.
	 */
	ts = zip->erd_size;
	zip->erd_size = archive_le16dec(p+12);
	__archive_read_consume(a, 14);
	if ((zip->erd_size & 0xf) != 0 ||
	    (zip->erd_size + 16) > remaining_size ||
	    (zip->erd_size + 16) < zip->erd_size)
		goto corrupted;

	if (ts < zip->erd_size) {
		free(zip->erd);
		zip->erd = NULL;
	}
	p = __archive_read_ahead(a, zip->erd_size, NULL);
	if (p == NULL)
		goto truncated;
	if (zip->erd == NULL) {
		zip->erd = malloc(zip->erd_size);
		if (zip->erd == NULL)
			goto nomem;
	}
	memcpy(zip->erd, p, zip->erd_size);
	__archive_read_consume(a, zip->erd_size);

	/*
	 * Read a reserved data field.
	 */
	p = __archive_read_ahead(a, 4, NULL);
	if (p == NULL)
		goto truncated;
	/* Reserved data size should be zero. */
	if (archive_le32dec(p) != 0)
		goto corrupted;
	__archive_read_consume(a, 4);

	/*
	 * Read a password validation data field.
	 */
	p = __archive_read_ahead(a, 2, NULL);
	if (p == NULL)
		goto truncated;
	ts = zip->v_size;
	zip->v_size = archive_le16dec(p);
	__archive_read_consume(a, 2);
	if ((zip->v_size & 0x0f) != 0 ||
	    (zip->erd_size + zip->v_size + 16) > remaining_size ||
	    (zip->erd_size + zip->v_size + 16) < (zip->erd_size + zip->v_size))
		goto corrupted;
	if (ts < zip->v_size) {
		free(zip->v_data);
		zip->v_data = NULL;
	}
	p = __archive_read_ahead(a, zip->v_size, NULL);
	if (p == NULL)
		goto truncated;
	if (zip->v_data == NULL) {
		zip->v_data = malloc(zip->v_size);
		if (zip->v_data == NULL)
			goto nomem;
	}
	memcpy(zip->v_data, p, zip->v_size);
	__archive_read_consume(a, zip->v_size);

	p = __archive_read_ahead(a, 4, NULL);
	if (p == NULL)
		goto truncated;
	zip->v_crc32 = archive_le32dec(p);
	__archive_read_consume(a, 4);

	/*return (ARCHIVE_OK);
	 * This is not fully implemnted yet.*/
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "Encrypted file is unsupported");
	return (ARCHIVE_FAILED);
truncated:
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "Truncated ZIP file data");
	return (ARCHIVE_FATAL);
corrupted:
	archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
	    "Corrupted ZIP file data");
	return (ARCHIVE_FATAL);
nomem:
	archive_set_error(&a->archive, ENOMEM,
	    "No memory for ZIP decryption");
	return (ARCHIVE_FATAL);
}
