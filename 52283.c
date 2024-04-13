set_error(struct archive_read *a, int ret)
{

	switch (ret) {
	case LZMA_STREAM_END: /* Found end of stream. */
	case LZMA_OK: /* Decompressor made some progress. */
		break;
	case LZMA_MEM_ERROR:
		archive_set_error(&a->archive, ENOMEM,
		    "Lzma library error: Cannot allocate memory");
		break;
	case LZMA_MEMLIMIT_ERROR:
		archive_set_error(&a->archive, ENOMEM,
		    "Lzma library error: Out of memory");
		break;
	case LZMA_FORMAT_ERROR:
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_MISC,
		    "Lzma library error: format not recognized");
		break;
	case LZMA_OPTIONS_ERROR:
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_MISC,
		    "Lzma library error: Invalid options");
		break;
	case LZMA_DATA_ERROR:
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_MISC,
		    "Lzma library error: Corrupted input data");
		break;
	case LZMA_BUF_ERROR:
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_MISC,
		    "Lzma library error:  No progress is possible");
		break;
	default:
		/* Return an error. */
		archive_set_error(&a->archive,
		    ARCHIVE_ERRNO_MISC,
		    "Lzma decompression failed:  Unknown error");
		break;
	}
}
