decompression_init(struct archive_read *a, enum enctype encoding)
{
	struct xar *xar;
	const char *detail;
	int r;

	xar = (struct xar *)(a->format->data);
	xar->rd_encoding = encoding;
	switch (encoding) {
	case NONE:
		break;
	case GZIP:
		if (xar->stream_valid)
			r = inflateReset(&(xar->stream));
		else
			r = inflateInit(&(xar->stream));
		if (r != Z_OK) {
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "Couldn't initialize zlib stream.");
			return (ARCHIVE_FATAL);
		}
		xar->stream_valid = 1;
		xar->stream.total_in = 0;
		xar->stream.total_out = 0;
		break;
#if defined(HAVE_BZLIB_H) && defined(BZ_CONFIG_ERROR)
	case BZIP2:
		if (xar->bzstream_valid) {
			BZ2_bzDecompressEnd(&(xar->bzstream));
			xar->bzstream_valid = 0;
		}
		r = BZ2_bzDecompressInit(&(xar->bzstream), 0, 0);
		if (r == BZ_MEM_ERROR)
			r = BZ2_bzDecompressInit(&(xar->bzstream), 0, 1);
		if (r != BZ_OK) {
			int err = ARCHIVE_ERRNO_MISC;
			detail = NULL;
			switch (r) {
			case BZ_PARAM_ERROR:
				detail = "invalid setup parameter";
				break;
			case BZ_MEM_ERROR:
				err = ENOMEM;
				detail = "out of memory";
				break;
			case BZ_CONFIG_ERROR:
				detail = "mis-compiled library";
				break;
			}
			archive_set_error(&a->archive, err,
			    "Internal error initializing decompressor: %s",
			    detail == NULL ? "??" : detail);
			xar->bzstream_valid = 0;
			return (ARCHIVE_FATAL);
		}
		xar->bzstream_valid = 1;
		xar->bzstream.total_in_lo32 = 0;
		xar->bzstream.total_in_hi32 = 0;
		xar->bzstream.total_out_lo32 = 0;
		xar->bzstream.total_out_hi32 = 0;
		break;
#endif
#if defined(HAVE_LZMA_H) && defined(HAVE_LIBLZMA)
#if LZMA_VERSION_MAJOR >= 5
/* Effectively disable the limiter. */
#define LZMA_MEMLIMIT   UINT64_MAX
#else
/* NOTE: This needs to check memory size which running system has. */
#define LZMA_MEMLIMIT   (1U << 30)
#endif
	case XZ:
	case LZMA:
		if (xar->lzstream_valid) {
			lzma_end(&(xar->lzstream));
			xar->lzstream_valid = 0;
		}
		if (xar->entry_encoding == XZ)
			r = lzma_stream_decoder(&(xar->lzstream),
			    LZMA_MEMLIMIT,/* memlimit */
			    LZMA_CONCATENATED);
		else
			r = lzma_alone_decoder(&(xar->lzstream),
			    LZMA_MEMLIMIT);/* memlimit */
		if (r != LZMA_OK) {
			switch (r) {
			case LZMA_MEM_ERROR:
				archive_set_error(&a->archive,
				    ENOMEM,
				    "Internal error initializing "
				    "compression library: "
				    "Cannot allocate memory");
				break;
			case LZMA_OPTIONS_ERROR:
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_MISC,
				    "Internal error initializing "
				    "compression library: "
				    "Invalid or unsupported options");
				break;
			default:
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_MISC,
				    "Internal error initializing "
				    "lzma library");
				break;
			}
			return (ARCHIVE_FATAL);
		}
		xar->lzstream_valid = 1;
		xar->lzstream.total_in = 0;
		xar->lzstream.total_out = 0;
		break;
#endif
	/*
	 * Unsupported compression.
	 */
	default:
#if !defined(HAVE_BZLIB_H) || !defined(BZ_CONFIG_ERROR)
	case BZIP2:
#endif
#if !defined(HAVE_LZMA_H) || !defined(HAVE_LIBLZMA)
	case LZMA:
	case XZ:
#endif
		switch (xar->entry_encoding) {
		case BZIP2: detail = "bzip2"; break;
		case LZMA: detail = "lzma"; break;
		case XZ: detail = "xz"; break;
		default: detail = "??"; break;
		}
		archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
		    "%s compression not supported on this platform",
		    detail);
		return (ARCHIVE_FAILED);
	}
	return (ARCHIVE_OK);
}
