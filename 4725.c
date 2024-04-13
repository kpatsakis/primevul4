static void wrap_in_tiff(deark *c, dbuf *f, i64 dpos, i64 dlen,
	const char *swstring, unsigned int tag, const char *ext, unsigned int createflags)
{
	dbuf *outf = NULL;
	i64 ifdoffs;
	i64 sw_len, sw_len_padded;
	i64 data_len_padded;

	sw_len = 1+(i64)de_strlen(swstring);
	if(sw_len<=4) return;
	sw_len_padded = de_pad_to_2(sw_len);

	if(dlen>4) {
		data_len_padded = de_pad_to_2(dlen);
	}
	else {
		data_len_padded = 0;
	}

	outf = dbuf_create_output_file(c, ext, NULL, 0);
	dbuf_write(outf, (const u8*)"\x4d\x4d\x00\x2a", 4);
	ifdoffs = 8 + sw_len_padded + data_len_padded;
	dbuf_writeu32be(outf, ifdoffs);
	dbuf_write(outf, (const u8*)swstring, sw_len);
	if(sw_len%2) dbuf_writebyte(outf, 0);
	if(dlen>4) {
		dbuf_copy(f, dpos, dlen, outf);
		if(dlen%2) dbuf_writebyte(outf, 0);
	}

	dbuf_writeu16be(outf, 2); // number of dir entries;

	dbuf_writeu16be(outf, 305); // Software tag
	dbuf_writeu16be(outf, 2); // type=ASCII
	dbuf_writeu32be(outf, sw_len);
	dbuf_writeu32be(outf, 8); // offset

	dbuf_writeu16be(outf, (i64)tag);
	dbuf_writeu16be(outf, 1);
	dbuf_writeu32be(outf, dlen);
	if(dlen>4) {
		dbuf_writeu32be(outf, 8+sw_len_padded);
	}
	else {
		dbuf_copy(f, dpos, dlen, outf);
		dbuf_write_zeroes(outf, 4-dlen);
	}

	dbuf_writeu32be(outf, 0); // end of IFD
	dbuf_close(outf);
}