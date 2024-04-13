zip_read_data_deflate(struct archive_read *a, const void **buff,
    size_t *size, int64_t *offset)
{
	struct zip *zip;
	ssize_t bytes_avail;
	const void *compressed_buff, *sp;
	int r;

	(void)offset; /* UNUSED */

	zip = (struct zip *)(a->format->data);

	/* If the buffer hasn't been allocated, allocate it now. */
	if (zip->uncompressed_buffer == NULL) {
		zip->uncompressed_buffer_size = 256 * 1024;
		zip->uncompressed_buffer
		    = (unsigned char *)malloc(zip->uncompressed_buffer_size);
		if (zip->uncompressed_buffer == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "No memory for ZIP decompression");
			return (ARCHIVE_FATAL);
		}
	}

	r = zip_deflate_init(a, zip);
	if (r != ARCHIVE_OK)
		return (r);

	/*
	 * Note: '1' here is a performance optimization.
	 * Recall that the decompression layer returns a count of
	 * available bytes; asking for more than that forces the
	 * decompressor to combine reads by copying data.
	 */
	compressed_buff = sp = __archive_read_ahead(a, 1, &bytes_avail);
	if (0 == (zip->entry->zip_flags & ZIP_LENGTH_AT_END)
	    && bytes_avail > zip->entry_bytes_remaining) {
		bytes_avail = (ssize_t)zip->entry_bytes_remaining;
	}
	if (bytes_avail <= 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated ZIP file body");
		return (ARCHIVE_FATAL);
	}

	if (zip->tctx_valid || zip->cctx_valid) {
		if (zip->decrypted_bytes_remaining < (size_t)bytes_avail) {
			size_t buff_remaining = zip->decrypted_buffer_size
			    - (zip->decrypted_ptr - zip->decrypted_buffer);

			if (buff_remaining > (size_t)bytes_avail)
				buff_remaining = (size_t)bytes_avail;

			if (0 == (zip->entry->zip_flags & ZIP_LENGTH_AT_END) &&
			      zip->entry_bytes_remaining > 0) {
				if ((int64_t)(zip->decrypted_bytes_remaining
				    + buff_remaining)
				      > zip->entry_bytes_remaining) {
					if (zip->entry_bytes_remaining <
					      (int64_t)zip->decrypted_bytes_remaining)
						buff_remaining = 0;
					else
						buff_remaining =
						    (size_t)zip->entry_bytes_remaining
						      - zip->decrypted_bytes_remaining;
				}
			}
			if (buff_remaining > 0) {
				if (zip->tctx_valid) {
					trad_enc_decrypt_update(&zip->tctx,
					    compressed_buff, buff_remaining,
					    zip->decrypted_ptr
					      + zip->decrypted_bytes_remaining,
					    buff_remaining);
				} else {
					size_t dsize = buff_remaining;
					archive_decrypto_aes_ctr_update(
					    &zip->cctx,
					    compressed_buff, buff_remaining,
					    zip->decrypted_ptr
					      + zip->decrypted_bytes_remaining,
					    &dsize);
				}
				zip->decrypted_bytes_remaining += buff_remaining;
			}
		}
		bytes_avail = zip->decrypted_bytes_remaining;
		compressed_buff = (const char *)zip->decrypted_ptr;
	}

	/*
	 * A bug in zlib.h: stream.next_in should be marked 'const'
	 * but isn't (the library never alters data through the
	 * next_in pointer, only reads it).  The result: this ugly
	 * cast to remove 'const'.
	 */
	zip->stream.next_in = (Bytef *)(uintptr_t)(const void *)compressed_buff;
	zip->stream.avail_in = (uInt)bytes_avail;
	zip->stream.total_in = 0;
	zip->stream.next_out = zip->uncompressed_buffer;
	zip->stream.avail_out = (uInt)zip->uncompressed_buffer_size;
	zip->stream.total_out = 0;

	r = inflate(&zip->stream, 0);
	switch (r) {
	case Z_OK:
		break;
	case Z_STREAM_END:
		zip->end_of_entry = 1;
		break;
	case Z_MEM_ERROR:
		archive_set_error(&a->archive, ENOMEM,
		    "Out of memory for ZIP decompression");
		return (ARCHIVE_FATAL);
	default:
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "ZIP decompression failed (%d)", r);
		return (ARCHIVE_FATAL);
	}

	/* Consume as much as the compressor actually used. */
	bytes_avail = zip->stream.total_in;
	if (zip->tctx_valid || zip->cctx_valid) {
		zip->decrypted_bytes_remaining -= bytes_avail;
		if (zip->decrypted_bytes_remaining == 0)
			zip->decrypted_ptr = zip->decrypted_buffer;
		else
			zip->decrypted_ptr += bytes_avail;
	}
	/* Calculate compressed data as much as we used.*/
	if (zip->hctx_valid)
		archive_hmac_sha1_update(&zip->hctx, sp, bytes_avail);
	__archive_read_consume(a, bytes_avail);
	zip->entry_bytes_remaining -= bytes_avail;
	zip->entry_compressed_bytes_read += bytes_avail;

	*size = zip->stream.total_out;
	zip->entry_uncompressed_bytes_read += zip->stream.total_out;
	*buff = zip->uncompressed_buffer;

	if (zip->end_of_entry && zip->hctx_valid) {
		r = check_authentication_code(a, NULL);
		if (r != ARCHIVE_OK)
			return (r);
	}

	if (zip->end_of_entry && (zip->entry->zip_flags & ZIP_LENGTH_AT_END)) {
		const char *p;

		if (NULL == (p = __archive_read_ahead(a, 24, NULL))) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Truncated ZIP end-of-file record");
			return (ARCHIVE_FATAL);
		}
		/* Consume the optional PK\007\010 marker. */
		if (p[0] == 'P' && p[1] == 'K' &&
		    p[2] == '\007' && p[3] == '\010') {
			p += 4;
			zip->unconsumed = 4;
		}
		if (zip->entry->flags & LA_USED_ZIP64) {
			zip->entry->crc32 = archive_le32dec(p);
			zip->entry->compressed_size = archive_le64dec(p + 4);
			zip->entry->uncompressed_size = archive_le64dec(p + 12);
			zip->unconsumed += 20;
		} else {
			zip->entry->crc32 = archive_le32dec(p);
			zip->entry->compressed_size = archive_le32dec(p + 4);
			zip->entry->uncompressed_size = archive_le32dec(p + 8);
			zip->unconsumed += 12;
		}
	}

	return (ARCHIVE_OK);
}
