seek_pack(struct archive_read *a)
{
	struct _7zip *zip = (struct _7zip *)a->format->data;
	int64_t pack_offset;

	if (zip->pack_stream_remaining <= 0) {
		archive_set_error(&(a->archive),
		    ARCHIVE_ERRNO_MISC, "Damaged 7-Zip archive");
		return (ARCHIVE_FATAL);
	}
	zip->pack_stream_inbytes_remaining =
	    zip->si.pi.sizes[zip->pack_stream_index];
	pack_offset = zip->si.pi.positions[zip->pack_stream_index];
	if (zip->stream_offset != pack_offset) {
		if (0 > __archive_read_seek(a, pack_offset + zip->seek_base,
		    SEEK_SET))
			return (ARCHIVE_FATAL);
		zip->stream_offset = pack_offset;
	}
	zip->pack_stream_index++;
	zip->pack_stream_remaining--;
	return (ARCHIVE_OK);
}
