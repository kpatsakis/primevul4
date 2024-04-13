decompress(struct archive_read *a, const void **buff, size_t *outbytes,
    const void *b, size_t *used)
{
	struct xar *xar;
	void *outbuff;
	size_t avail_in, avail_out;
	int r;

	xar = (struct xar *)(a->format->data);
	avail_in = *used;
	outbuff = (void *)(uintptr_t)*buff;
	if (outbuff == NULL) {
		if (xar->outbuff == NULL) {
			xar->outbuff = malloc(OUTBUFF_SIZE);
			if (xar->outbuff == NULL) {
				archive_set_error(&a->archive, ENOMEM,
				    "Couldn't allocate memory for out buffer");
				return (ARCHIVE_FATAL);
			}
		}
		outbuff = xar->outbuff;
		*buff = outbuff;
		avail_out = OUTBUFF_SIZE;
	} else
		avail_out = *outbytes;
	switch (xar->rd_encoding) {
	case GZIP:
		xar->stream.next_in = (Bytef *)(uintptr_t)b;
		xar->stream.avail_in = avail_in;
		xar->stream.next_out = (unsigned char *)outbuff;
		xar->stream.avail_out = avail_out;
		r = inflate(&(xar->stream), 0);
		switch (r) {
		case Z_OK: /* Decompressor made some progress.*/
		case Z_STREAM_END: /* Found end of stream. */
			break;
		default:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "File decompression failed (%d)", r);
			return (ARCHIVE_FATAL);
		}
		*used = avail_in - xar->stream.avail_in;
		*outbytes = avail_out - xar->stream.avail_out;
		break;
#if defined(HAVE_BZLIB_H) && defined(BZ_CONFIG_ERROR)
	case BZIP2:
		xar->bzstream.next_in = (char *)(uintptr_t)b;
		xar->bzstream.avail_in = avail_in;
		xar->bzstream.next_out = (char *)outbuff;
		xar->bzstream.avail_out = avail_out;
		r = BZ2_bzDecompress(&(xar->bzstream));
		switch (r) {
		case BZ_STREAM_END: /* Found end of stream. */
			switch (BZ2_bzDecompressEnd(&(xar->bzstream))) {
			case BZ_OK:
				break;
			default:
				archive_set_error(&(a->archive),
				    ARCHIVE_ERRNO_MISC,
				    "Failed to clean up decompressor");
				return (ARCHIVE_FATAL);
			}
			xar->bzstream_valid = 0;
			/* FALLTHROUGH */
		case BZ_OK: /* Decompressor made some progress. */
			break;
		default:
			archive_set_error(&(a->archive),
			    ARCHIVE_ERRNO_MISC,
			    "bzip decompression failed");
			return (ARCHIVE_FATAL);
		}
		*used = avail_in - xar->bzstream.avail_in;
		*outbytes = avail_out - xar->bzstream.avail_out;
		break;
#endif
#if defined(HAVE_LZMA_H) && defined(HAVE_LIBLZMA)
	case LZMA:
	case XZ:
		xar->lzstream.next_in = b;
		xar->lzstream.avail_in = avail_in;
		xar->lzstream.next_out = (unsigned char *)outbuff;
		xar->lzstream.avail_out = avail_out;
		r = lzma_code(&(xar->lzstream), LZMA_RUN);
		switch (r) {
		case LZMA_STREAM_END: /* Found end of stream. */
			lzma_end(&(xar->lzstream));
			xar->lzstream_valid = 0;
			/* FALLTHROUGH */
		case LZMA_OK: /* Decompressor made some progress. */
			break;
		default:
			archive_set_error(&(a->archive),
			    ARCHIVE_ERRNO_MISC,
			    "%s decompression failed(%d)",
			    (xar->entry_encoding == XZ)?"xz":"lzma",
			    r);
			return (ARCHIVE_FATAL);
		}
		*used = avail_in - xar->lzstream.avail_in;
		*outbytes = avail_out - xar->lzstream.avail_out;
		break;
#endif
#if !defined(HAVE_BZLIB_H) || !defined(BZ_CONFIG_ERROR)
	case BZIP2:
#endif
#if !defined(HAVE_LZMA_H) || !defined(HAVE_LIBLZMA)
	case LZMA:
	case XZ:
#endif
	case NONE:
	default:
		if (outbuff == xar->outbuff) {
			*buff = b;
			*used = avail_in;
			*outbytes = avail_in;
		} else {
			if (avail_out > avail_in)
				avail_out = avail_in;
			memcpy(outbuff, b, avail_out);
			*used = avail_out;
			*outbytes = avail_out;
		}
		break;
	}
	return (ARCHIVE_OK);
}
