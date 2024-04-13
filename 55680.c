decompression_cleanup(struct archive_read *a)
{
	struct xar *xar;
	int r;

	xar = (struct xar *)(a->format->data);
	r = ARCHIVE_OK;
	if (xar->stream_valid) {
		if (inflateEnd(&(xar->stream)) != Z_OK) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_MISC,
			    "Failed to clean up zlib decompressor");
			r = ARCHIVE_FATAL;
		}
	}
#if defined(HAVE_BZLIB_H) && defined(BZ_CONFIG_ERROR)
	if (xar->bzstream_valid) {
		if (BZ2_bzDecompressEnd(&(xar->bzstream)) != BZ_OK) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_MISC,
			    "Failed to clean up bzip2 decompressor");
			r = ARCHIVE_FATAL;
		}
	}
#endif
#if defined(HAVE_LZMA_H) && defined(HAVE_LIBLZMA)
	if (xar->lzstream_valid)
		lzma_end(&(xar->lzstream));
#elif defined(HAVE_LZMA_H) && defined(HAVE_LIBLZMA)
	if (xar->lzstream_valid) {
		if (lzmadec_end(&(xar->lzstream)) != LZMADEC_OK) {
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_MISC,
			    "Failed to clean up lzmadec decompressor");
			r = ARCHIVE_FATAL;
		}
	}
#endif
	return (r);
}
