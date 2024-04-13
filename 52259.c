decompress(struct archive_read *a, struct _7zip *zip,
    void *buff, size_t *outbytes, const void *b, size_t *used)
{
	const uint8_t *t_next_in;
	uint8_t *t_next_out;
	size_t o_avail_in, o_avail_out;
	size_t t_avail_in, t_avail_out;
	uint8_t *bcj2_next_out;
	size_t bcj2_avail_out;
	int r, ret = ARCHIVE_OK;

	t_avail_in = o_avail_in = *used;
	t_avail_out = o_avail_out = *outbytes;
	t_next_in = b;
	t_next_out = buff;

	if (zip->codec != _7Z_LZMA2 && zip->codec2 == _7Z_X86) {
		int i;

		/* Do not copy out the BCJ remaining bytes when the output
		 * buffer size is less than five bytes. */
		if (o_avail_in != 0 && t_avail_out < 5 && zip->odd_bcj_size) {
			*used = 0;
			*outbytes = 0;
			return (ret);
		}
		for (i = 0; zip->odd_bcj_size > 0 && t_avail_out; i++) {
			*t_next_out++ = zip->odd_bcj[i];
			t_avail_out--;
			zip->odd_bcj_size--;
		}
		if (o_avail_in == 0 || t_avail_out == 0) {
			*used = o_avail_in - t_avail_in;
			*outbytes = o_avail_out - t_avail_out;
			if (o_avail_in == 0)
				ret = ARCHIVE_EOF;
			return (ret);
		}
	}

	bcj2_next_out = t_next_out;
	bcj2_avail_out = t_avail_out;
	if (zip->codec2 == _7Z_X86_BCJ2) {
		/*
		 * Decord a remaining decompressed main stream for BCJ2.
		 */
		if (zip->tmp_stream_bytes_remaining) {
			ssize_t bytes;
			size_t remaining = zip->tmp_stream_bytes_remaining;
			bytes = Bcj2_Decode(zip, t_next_out, t_avail_out);
			if (bytes < 0) {
				archive_set_error(&(a->archive),
				    ARCHIVE_ERRNO_MISC,
				    "BCJ2 conversion Failed");
				return (ARCHIVE_FAILED);
			}
			zip->main_stream_bytes_remaining -=
			    remaining - zip->tmp_stream_bytes_remaining;
			t_avail_out -= bytes;
			if (o_avail_in == 0 || t_avail_out == 0) {
				*used = 0;
				*outbytes = o_avail_out - t_avail_out;
				if (o_avail_in == 0 &&
				    zip->tmp_stream_bytes_remaining)
					ret = ARCHIVE_EOF;
				return (ret);
			}
			t_next_out += bytes;
			bcj2_next_out = t_next_out;
			bcj2_avail_out = t_avail_out;
		}
		t_next_out = zip->tmp_stream_buff;
		t_avail_out = zip->tmp_stream_buff_size;
	}

	switch (zip->codec) {
	case _7Z_COPY:
	{
		size_t bytes =
		    (t_avail_in > t_avail_out)?t_avail_out:t_avail_in;

		memcpy(t_next_out, t_next_in, bytes);
		t_avail_in -= bytes;
		t_avail_out -= bytes;
		if (o_avail_in == 0)
			ret = ARCHIVE_EOF;
		break;
	}
#ifdef HAVE_LZMA_H
	case _7Z_LZMA: case _7Z_LZMA2:
		zip->lzstream.next_in = t_next_in;
		zip->lzstream.avail_in = t_avail_in;
		zip->lzstream.next_out = t_next_out;
		zip->lzstream.avail_out = t_avail_out;

		r = lzma_code(&(zip->lzstream), LZMA_RUN);
		switch (r) {
		case LZMA_STREAM_END: /* Found end of stream. */
			lzma_end(&(zip->lzstream));
			zip->lzstream_valid = 0;
			ret = ARCHIVE_EOF;
			break;
		case LZMA_OK: /* Decompressor made some progress. */
			break;
		default:
			archive_set_error(&(a->archive),
			    ARCHIVE_ERRNO_MISC,
				"Decompression failed(%d)",
			    r);
			return (ARCHIVE_FAILED);
		}
		t_avail_in = zip->lzstream.avail_in;
		t_avail_out = zip->lzstream.avail_out;
		break;
#endif
#if defined(HAVE_BZLIB_H) && defined(BZ_CONFIG_ERROR)
	case _7Z_BZ2:
		zip->bzstream.next_in = (char *)(uintptr_t)t_next_in;
		zip->bzstream.avail_in = t_avail_in;
		zip->bzstream.next_out = (char *)(uintptr_t)t_next_out;
		zip->bzstream.avail_out = t_avail_out;
		r = BZ2_bzDecompress(&(zip->bzstream));
		switch (r) {
		case BZ_STREAM_END: /* Found end of stream. */
			switch (BZ2_bzDecompressEnd(&(zip->bzstream))) {
			case BZ_OK:
				break;
			default:
				archive_set_error(&(a->archive),
				    ARCHIVE_ERRNO_MISC,
				    "Failed to clean up decompressor");
				return (ARCHIVE_FAILED);
			}
			zip->bzstream_valid = 0;
			ret = ARCHIVE_EOF;
			break;
		case BZ_OK: /* Decompressor made some progress. */
			break;
		default:
			archive_set_error(&(a->archive),
			    ARCHIVE_ERRNO_MISC,
			    "bzip decompression failed");
			return (ARCHIVE_FAILED);
		}
		t_avail_in = zip->bzstream.avail_in;
		t_avail_out = zip->bzstream.avail_out;
		break;
#endif
#ifdef HAVE_ZLIB_H
	case _7Z_DEFLATE:
		zip->stream.next_in = (Bytef *)(uintptr_t)t_next_in;
		zip->stream.avail_in = (uInt)t_avail_in;
		zip->stream.next_out = t_next_out;
		zip->stream.avail_out = (uInt)t_avail_out;
		r = inflate(&(zip->stream), 0);
		switch (r) {
		case Z_STREAM_END: /* Found end of stream. */
			ret = ARCHIVE_EOF;
			break;
		case Z_OK: /* Decompressor made some progress.*/
			break;
		default:
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "File decompression failed (%d)", r);
			return (ARCHIVE_FAILED);
		}
		t_avail_in = zip->stream.avail_in;
		t_avail_out = zip->stream.avail_out;
		break;
#endif
	case _7Z_PPMD:
	{
		uint64_t flush_bytes;

		if (!zip->ppmd7_valid || zip->ppmd7_stat < 0 ||
		    t_avail_out <= 0) {
			archive_set_error(&(a->archive),
			    ARCHIVE_ERRNO_MISC,
			    "Decompression internal error");
			return (ARCHIVE_FAILED);
		}
		zip->ppstream.next_in = t_next_in;
		zip->ppstream.avail_in = t_avail_in;
		zip->ppstream.next_out = t_next_out;
		zip->ppstream.avail_out = t_avail_out;
		if (zip->ppmd7_stat == 0) {
			zip->bytein.a = a;
			zip->bytein.Read = &ppmd_read;
			zip->range_dec.Stream = &zip->bytein;
			r = __archive_ppmd7_functions.Ppmd7z_RangeDec_Init(
				&(zip->range_dec));
			if (r == 0) {
				zip->ppmd7_stat = -1;
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_MISC,
				    "Failed to initialize PPMd range decorder");
				return (ARCHIVE_FAILED);
			}
			if (zip->ppstream.overconsumed) {
				zip->ppmd7_stat = -1;
				return (ARCHIVE_FAILED);
			}
			zip->ppmd7_stat = 1;
		}

		if (t_avail_in == 0)
			/* XXX Flush out remaining decoded data XXX */
			flush_bytes = zip->folder_outbytes_remaining;
		else
			flush_bytes = 0;

		do {
			int sym;

			sym = __archive_ppmd7_functions.Ppmd7_DecodeSymbol(
				&(zip->ppmd7_context), &(zip->range_dec.p));
			if (sym < 0) {
				zip->ppmd7_stat = -1;
				archive_set_error(&a->archive,
				    ARCHIVE_ERRNO_FILE_FORMAT,
				    "Failed to decode PPMd");
				return (ARCHIVE_FAILED);
			}
			if (zip->ppstream.overconsumed) {
				zip->ppmd7_stat = -1;
				return (ARCHIVE_FAILED);
			}
			*zip->ppstream.next_out++ = (unsigned char)sym;
			zip->ppstream.avail_out--;
			zip->ppstream.total_out++;
			if (flush_bytes)
				flush_bytes--;
		} while (zip->ppstream.avail_out &&
			(zip->ppstream.avail_in || flush_bytes));

		t_avail_in = (size_t)zip->ppstream.avail_in;
		t_avail_out = (size_t)zip->ppstream.avail_out;
		break;
	}
	default:
		archive_set_error(&(a->archive), ARCHIVE_ERRNO_MISC,
		    "Decompression internal error");
		return (ARCHIVE_FAILED);
	}
	if (ret != ARCHIVE_OK && ret != ARCHIVE_EOF)
		return (ret);

	*used = o_avail_in - t_avail_in;
	*outbytes = o_avail_out - t_avail_out;

	/*
	 * Decord BCJ.
	 */
	if (zip->codec != _7Z_LZMA2 && zip->codec2 == _7Z_X86) {
		size_t l = x86_Convert(zip, buff, *outbytes);
		zip->odd_bcj_size = *outbytes - l;
		if (zip->odd_bcj_size > 0 && zip->odd_bcj_size <= 4 &&
		    o_avail_in && ret != ARCHIVE_EOF) {
			memcpy(zip->odd_bcj, ((unsigned char *)buff) + l,
			    zip->odd_bcj_size);
			*outbytes = l;
		} else
			zip->odd_bcj_size = 0;
	}

	/*
	 * Decord BCJ2 with a decompressed main stream.
	 */
	if (zip->codec2 == _7Z_X86_BCJ2) {
		ssize_t bytes;

		zip->tmp_stream_bytes_avail =
		    zip->tmp_stream_buff_size - t_avail_out;
		if (zip->tmp_stream_bytes_avail >
		      zip->main_stream_bytes_remaining)
			zip->tmp_stream_bytes_avail =
			    zip->main_stream_bytes_remaining;
		zip->tmp_stream_bytes_remaining = zip->tmp_stream_bytes_avail;
		bytes = Bcj2_Decode(zip, bcj2_next_out, bcj2_avail_out);
		if (bytes < 0) {
			archive_set_error(&(a->archive),
			    ARCHIVE_ERRNO_MISC, "BCJ2 conversion Failed");
			return (ARCHIVE_FAILED);
		}
		zip->main_stream_bytes_remaining -=
		    zip->tmp_stream_bytes_avail
		      - zip->tmp_stream_bytes_remaining;
		bcj2_avail_out -= bytes;
		*outbytes = o_avail_out - bcj2_avail_out;
	}

	return (ret);
}
