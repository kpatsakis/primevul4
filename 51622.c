zisofs_read_data(struct archive_read *a,
    const void **buff, size_t *size, int64_t *offset)
{
	struct iso9660 *iso9660;
	struct zisofs  *zisofs;
	const unsigned char *p;
	size_t avail;
	ssize_t bytes_read;
	size_t uncompressed_size;
	int r;

	iso9660 = (struct iso9660 *)(a->format->data);
	zisofs = &iso9660->entry_zisofs;

	p = __archive_read_ahead(a, 1, &bytes_read);
	if (bytes_read <= 0) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "Truncated zisofs file body");
		return (ARCHIVE_FATAL);
	}
	if (bytes_read > iso9660->entry_bytes_remaining)
		bytes_read = (ssize_t)iso9660->entry_bytes_remaining;
	avail = bytes_read;
	uncompressed_size = 0;

	if (!zisofs->initialized) {
		size_t ceil, xsize;

		/* Allocate block pointers buffer. */
		ceil = (size_t)((zisofs->pz_uncompressed_size +
			(((int64_t)1) << zisofs->pz_log2_bs) - 1)
			>> zisofs->pz_log2_bs);
		xsize = (ceil + 1) * 4;
		if (zisofs->block_pointers_alloc < xsize) {
			size_t alloc;

			if (zisofs->block_pointers != NULL)
				free(zisofs->block_pointers);
			alloc = ((xsize >> 10) + 1) << 10;
			zisofs->block_pointers = malloc(alloc);
			if (zisofs->block_pointers == NULL) {
				archive_set_error(&a->archive, ENOMEM,
				    "No memory for zisofs decompression");
				return (ARCHIVE_FATAL);
			}
			zisofs->block_pointers_alloc = alloc;
		}
		zisofs->block_pointers_size = xsize;

		/* Allocate uncompressed data buffer. */
		xsize = (size_t)1UL << zisofs->pz_log2_bs;
		if (zisofs->uncompressed_buffer_size < xsize) {
			if (zisofs->uncompressed_buffer != NULL)
				free(zisofs->uncompressed_buffer);
			zisofs->uncompressed_buffer = malloc(xsize);
			if (zisofs->uncompressed_buffer == NULL) {
				archive_set_error(&a->archive, ENOMEM,
				    "No memory for zisofs decompression");
				return (ARCHIVE_FATAL);
			}
		}
		zisofs->uncompressed_buffer_size = xsize;

		/*
		 * Read the file header, and check the magic code of zisofs.
		 */
		if (zisofs->header_avail < sizeof(zisofs->header)) {
			xsize = sizeof(zisofs->header) - zisofs->header_avail;
			if (avail < xsize)
				xsize = avail;
			memcpy(zisofs->header + zisofs->header_avail, p, xsize);
			zisofs->header_avail += xsize;
			avail -= xsize;
			p += xsize;
		}
		if (!zisofs->header_passed &&
		    zisofs->header_avail == sizeof(zisofs->header)) {
			int err = 0;

			if (memcmp(zisofs->header, zisofs_magic,
			    sizeof(zisofs_magic)) != 0)
				err = 1;
			if (archive_le32dec(zisofs->header + 8)
			    != zisofs->pz_uncompressed_size)
				err = 1;
			if (zisofs->header[12] != 4)
				err = 1;
			if (zisofs->header[13] != zisofs->pz_log2_bs)
				err = 1;
			if (err) {
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_FILE_FORMAT,
				    "Illegal zisofs file body");
				return (ARCHIVE_FATAL);
			}
			zisofs->header_passed = 1;
		}
		/*
		 * Read block pointers.
		 */
		if (zisofs->header_passed &&
		    zisofs->block_pointers_avail < zisofs->block_pointers_size) {
			xsize = zisofs->block_pointers_size
			    - zisofs->block_pointers_avail;
			if (avail < xsize)
				xsize = avail;
			memcpy(zisofs->block_pointers
			    + zisofs->block_pointers_avail, p, xsize);
			zisofs->block_pointers_avail += xsize;
			avail -= xsize;
			p += xsize;
		    	if (zisofs->block_pointers_avail
			    == zisofs->block_pointers_size) {
				/* We've got all block pointers and initialize
				 * related variables.	*/
				zisofs->block_off = 0;
				zisofs->block_avail = 0;
				/* Complete a initialization */
				zisofs->initialized = 1;
			}
		}

		if (!zisofs->initialized)
			goto next_data; /* We need more data. */
	}

	/*
	 * Get block offsets from block pointers.
	 */
	if (zisofs->block_avail == 0) {
		uint32_t bst, bed;

		if (zisofs->block_off + 4 >= zisofs->block_pointers_size) {
			/* There isn't a pair of offsets. */
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Illegal zisofs block pointers");
			return (ARCHIVE_FATAL);
		}
		bst = archive_le32dec(
		    zisofs->block_pointers + zisofs->block_off);
		if (bst != zisofs->pz_offset + (bytes_read - avail)) {
			/* TODO: Should we seek offset of current file
			 * by bst ? */
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Illegal zisofs block pointers(cannot seek)");
			return (ARCHIVE_FATAL);
		}
		bed = archive_le32dec(
		    zisofs->block_pointers + zisofs->block_off + 4);
		if (bed < bst) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Illegal zisofs block pointers");
			return (ARCHIVE_FATAL);
		}
		zisofs->block_avail = bed - bst;
		zisofs->block_off += 4;

		/* Initialize compression library for new block. */
		if (zisofs->stream_valid)
			r = inflateReset(&zisofs->stream);
		else
			r = inflateInit(&zisofs->stream);
		if (r != Z_OK) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Can't initialize zisofs decompression.");
			return (ARCHIVE_FATAL);
		}
		zisofs->stream_valid = 1;
		zisofs->stream.total_in = 0;
		zisofs->stream.total_out = 0;
	}

	/*
	 * Make uncompressed data.
	 */
	if (zisofs->block_avail == 0) {
		memset(zisofs->uncompressed_buffer, 0,
		    zisofs->uncompressed_buffer_size);
		uncompressed_size = zisofs->uncompressed_buffer_size;
	} else {
		zisofs->stream.next_in = (Bytef *)(uintptr_t)(const void *)p;
		if (avail > zisofs->block_avail)
			zisofs->stream.avail_in = zisofs->block_avail;
		else
			zisofs->stream.avail_in = (uInt)avail;
		zisofs->stream.next_out = zisofs->uncompressed_buffer;
		zisofs->stream.avail_out =
		    (uInt)zisofs->uncompressed_buffer_size;

		r = inflate(&zisofs->stream, 0);
		switch (r) {
		case Z_OK: /* Decompressor made some progress.*/
		case Z_STREAM_END: /* Found end of stream. */
			break;
		default:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "zisofs decompression failed (%d)", r);
			return (ARCHIVE_FATAL);
		}
		uncompressed_size =
		    zisofs->uncompressed_buffer_size - zisofs->stream.avail_out;
		avail -= zisofs->stream.next_in - p;
		zisofs->block_avail -= (uint32_t)(zisofs->stream.next_in - p);
	}
next_data:
	bytes_read -= avail;
	*buff = zisofs->uncompressed_buffer;
	*size = uncompressed_size;
	*offset = iso9660->entry_sparse_offset;
	iso9660->entry_sparse_offset += uncompressed_size;
	iso9660->entry_bytes_remaining -= bytes_read;
	iso9660->current_position += bytes_read;
	zisofs->pz_offset += (uint32_t)bytes_read;
	iso9660->entry_bytes_unconsumed += bytes_read;

	return (ARCHIVE_OK);
}
