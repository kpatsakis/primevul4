extract_pack_stream(struct archive_read *a, size_t minimum)
{
	struct _7zip *zip = (struct _7zip *)a->format->data;
	ssize_t bytes_avail;
	int r;

	if (zip->codec == _7Z_COPY && zip->codec2 == (unsigned long)-1) {
		if (minimum == 0)
			minimum = 1;
		if (__archive_read_ahead(a, minimum, &bytes_avail) == NULL
		    || bytes_avail <= 0) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Truncated 7-Zip file body");
			return (ARCHIVE_FATAL);
		}
		if (bytes_avail > (ssize_t)zip->pack_stream_inbytes_remaining)
			bytes_avail = (ssize_t)zip->pack_stream_inbytes_remaining;
		zip->pack_stream_inbytes_remaining -= bytes_avail;
		if (bytes_avail > (ssize_t)zip->folder_outbytes_remaining)
			bytes_avail = (ssize_t)zip->folder_outbytes_remaining;
		zip->folder_outbytes_remaining -= bytes_avail;
		zip->uncompressed_buffer_bytes_remaining = bytes_avail;
		return (ARCHIVE_OK);
	}

	/* If the buffer hasn't been allocated, allocate it now. */
	if (zip->uncompressed_buffer == NULL) {
		zip->uncompressed_buffer_size = UBUFF_SIZE;
		if (zip->uncompressed_buffer_size < minimum) {
			zip->uncompressed_buffer_size = minimum + 1023;
			zip->uncompressed_buffer_size &= ~0x3ff;
		}
		zip->uncompressed_buffer =
		    malloc(zip->uncompressed_buffer_size);
		if (zip->uncompressed_buffer == NULL) {
			archive_set_error(&a->archive, ENOMEM,
			    "No memory for 7-Zip decompression");
			return (ARCHIVE_FATAL);
		}
		zip->uncompressed_buffer_bytes_remaining = 0;
	} else if (zip->uncompressed_buffer_size < minimum ||
	    zip->uncompressed_buffer_bytes_remaining < minimum) {
		/*
		 * Make sure the uncompressed buffer can have bytes
		 * at least `minimum' bytes.
		 * NOTE: This case happen when reading the header.
		 */
		size_t used;
		if (zip->uncompressed_buffer_pointer != 0)
			used = zip->uncompressed_buffer_pointer -
				zip->uncompressed_buffer;
		else
			used = 0;
		if (zip->uncompressed_buffer_size < minimum) {
			/*
			 * Expand the uncompressed buffer up to
			 * the minimum size.
			 */
			void *p;
			size_t new_size;

			new_size = minimum + 1023;
			new_size &= ~0x3ff;
			p = realloc(zip->uncompressed_buffer, new_size);
			if (p == NULL) {
				archive_set_error(&a->archive, ENOMEM,
				    "No memory for 7-Zip decompression");
				return (ARCHIVE_FATAL);
			}
			zip->uncompressed_buffer = (unsigned char *)p;
			zip->uncompressed_buffer_size = new_size;
		}
		/*
		 * Move unconsumed bytes to the head.
		 */
		if (used) {
			memmove(zip->uncompressed_buffer,
				zip->uncompressed_buffer + used,
				zip->uncompressed_buffer_bytes_remaining);
		}
	} else
		zip->uncompressed_buffer_bytes_remaining = 0;
	zip->uncompressed_buffer_pointer = NULL;
	for (;;) {
		size_t bytes_in, bytes_out;
		const void *buff_in;
		unsigned char *buff_out;
		int end_of_data;

		/*
		 * Note: '1' here is a performance optimization.
		 * Recall that the decompression layer returns a count of
		 * available bytes; asking for more than that forces the
		 * decompressor to combine reads by copying data.
		 */
		buff_in = __archive_read_ahead(a, 1, &bytes_avail);
		if (bytes_avail <= 0) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Truncated 7-Zip file body");
			return (ARCHIVE_FATAL);
		}

		buff_out = zip->uncompressed_buffer
			+ zip->uncompressed_buffer_bytes_remaining;
		bytes_out = zip->uncompressed_buffer_size
			- zip->uncompressed_buffer_bytes_remaining;
		bytes_in = bytes_avail;
		if (bytes_in > zip->pack_stream_inbytes_remaining)
			bytes_in = (size_t)zip->pack_stream_inbytes_remaining;
		/* Drive decompression. */
		r = decompress(a, zip, buff_out, &bytes_out,
			buff_in, &bytes_in);
		switch (r) {
		case ARCHIVE_OK:
			end_of_data = 0;
			break;
		case ARCHIVE_EOF:
			end_of_data = 1;
			break;
		default:
			return (ARCHIVE_FATAL);
		}
		zip->pack_stream_inbytes_remaining -= bytes_in;
		if (bytes_out > zip->folder_outbytes_remaining)
			bytes_out = (size_t)zip->folder_outbytes_remaining;
		zip->folder_outbytes_remaining -= bytes_out;
		zip->uncompressed_buffer_bytes_remaining += bytes_out;
		zip->pack_stream_bytes_unconsumed = bytes_in;

		/*
		 * Continue decompression until uncompressed_buffer is full.
		 */
		if (zip->uncompressed_buffer_bytes_remaining ==
		    zip->uncompressed_buffer_size)
			break;
		if (zip->codec2 == _7Z_X86 && zip->odd_bcj_size &&
		    zip->uncompressed_buffer_bytes_remaining + 5 >
		    zip->uncompressed_buffer_size)
			break;
		if (zip->pack_stream_inbytes_remaining == 0 &&
		    zip->folder_outbytes_remaining == 0)
			break;
		if (end_of_data || (bytes_in == 0 && bytes_out == 0)) {
			archive_set_error(&(a->archive),
			    ARCHIVE_ERRNO_MISC, "Damaged 7-Zip archive");
			return (ARCHIVE_FATAL);
		}
		read_consume(a);
	}
	if (zip->uncompressed_buffer_bytes_remaining < minimum) {
		archive_set_error(&(a->archive),
		    ARCHIVE_ERRNO_MISC, "Damaged 7-Zip archive");
		return (ARCHIVE_FATAL);
	}
	zip->uncompressed_buffer_pointer = zip->uncompressed_buffer;
	return (ARCHIVE_OK);
}
