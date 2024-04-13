void fmtutil_generate_bmpfileheader(deark *c, dbuf *outf, const struct de_bmpinfo *bi,
	i64 file_size_override)
{
	i64 file_size_to_write;

	dbuf_write(outf, (const u8*)"BM", 2);

	if(file_size_override)
		file_size_to_write = file_size_override;
	else
		file_size_to_write = 14 + bi->total_size;
	dbuf_writeu32le(outf, file_size_to_write);

	dbuf_write_zeroes(outf, 4);
	dbuf_writeu32le(outf, 14 + bi->size_of_headers_and_pal);
}