int fmtutil_get_bmpinfo(deark *c, dbuf *f, struct de_bmpinfo *bi, i64 pos,
	i64 len, unsigned int flags)
{
	i64 fhs; // file header size
	i64 bmih_pos;
	struct de_fourcc cmpr4cc;
	char cmprname_dbgstr[80];

	de_zeromem(bi, sizeof(struct de_bmpinfo));
	de_zeromem(&cmpr4cc, sizeof(struct de_fourcc));

	fhs = (flags & DE_BMPINFO_HAS_FILEHEADER) ? 14 : 0;

	if(fhs+len < 16) return 0;

	if(fhs) {
		if(flags & DE_BMPINFO_HAS_HOTSPOT) {
			bi->hotspot_x = (int)dbuf_getu16le(f, pos+6);
			bi->hotspot_y = (int)dbuf_getu16le(f, pos+8);
			de_dbg(c, "hotspot: (%d,%d)", bi->hotspot_x, bi->hotspot_y);
		}

		bi->bitsoffset = dbuf_getu32le(f, pos+10);
		de_dbg(c, "bits offset: %d", (int)bi->bitsoffset);
	}

	bmih_pos = pos + fhs;

	bi->infohdrsize = dbuf_getu32le(f, bmih_pos);

	if(bi->infohdrsize==0x474e5089 && (flags & DE_BMPINFO_ICO_FORMAT)) {
		// We don't examine PNG-formatted icons, but we can identify them.
		bi->infohdrsize = 0;
		bi->file_format = DE_BMPINFO_FMT_PNG;
		return 1;
	}

	de_dbg(c, "info header size: %d", (int)bi->infohdrsize);

	if(bi->infohdrsize==12) {
		bi->bytes_per_pal_entry = 3;
		bi->width = dbuf_getu16le(f, bmih_pos+4);
		bi->height = dbuf_getu16le(f, bmih_pos+6);
		bi->bitcount = dbuf_getu16le(f, bmih_pos+10);
	}
	else if(bi->infohdrsize>=16 && bi->infohdrsize<=124) {
		bi->bytes_per_pal_entry = 4;
		bi->width = dbuf_getu32le(f, bmih_pos+4);
		bi->height = dbuf_geti32le(f, bmih_pos+8);
		if(bi->height<0) {
			bi->is_topdown = 1;
			bi->height = -bi->height;
		}
		bi->bitcount = dbuf_getu16le(f, bmih_pos+14);
		if(bi->infohdrsize>=20) {
			bi->compression_field = (u32)dbuf_getu32le(f, bmih_pos+16);
			if(flags & DE_BMPINFO_CMPR_IS_4CC) {
				dbuf_read_fourcc(f, bmih_pos+16, &cmpr4cc, 4, 0x0);
			}
		}
		if(bi->infohdrsize>=24) {
			bi->sizeImage_field = dbuf_getu32le(f, bmih_pos+20);
		}
		if(bi->infohdrsize>=36) {
			bi->pal_entries = dbuf_getu32le(f, bmih_pos+32);
		}
	}
	else {
		return 0;
	}

	if(flags & DE_BMPINFO_ICO_FORMAT) bi->height /= 2;

	if(bi->bitcount>=1 && bi->bitcount<=8) {
		if(bi->pal_entries==0) {
			bi->pal_entries = de_pow2(bi->bitcount);
		}
		// I think the NumColors field (in icons) is supposed to be the maximum number of
		// colors implied by the bit depth, not the number of colors in the palette.
		bi->num_colors = de_pow2(bi->bitcount);
	}
	else {
		// An arbitrary value. All that matters is that it's >=256.
		bi->num_colors = 16777216;
	}

	de_dbg_dimensions(c, bi->width, bi->height);
	de_dbg(c, "bit count: %d", (int)bi->bitcount);

	if((flags & DE_BMPINFO_CMPR_IS_4CC) && (bi->compression_field>0xffff)) {
		de_snprintf(cmprname_dbgstr, sizeof(cmprname_dbgstr), "'%s'", cmpr4cc.id_dbgstr);
	}
	else {
		fmtutil_get_bmp_compression_name(bi->compression_field,
			cmprname_dbgstr, sizeof(cmprname_dbgstr), 0);
	}
	de_dbg(c, "compression: %u (%s)", (unsigned int)bi->compression_field, cmprname_dbgstr);

	if(bi->sizeImage_field!=0) {
		de_dbg(c, "sizeImage: %u", (unsigned int)bi->sizeImage_field);
	}

	de_dbg(c, "palette entries: %u", (unsigned int)bi->pal_entries);
	if(bi->pal_entries>256 && bi->bitcount>8) {
		de_warn(c, "Ignoring bad palette size (%u entries)", (unsigned int)bi->pal_entries);
		bi->pal_entries = 0;
	}

	bi->pal_bytes = bi->bytes_per_pal_entry*bi->pal_entries;
	bi->size_of_headers_and_pal = fhs + bi->infohdrsize + bi->pal_bytes;

	// FIXME: cmpr type 3 doesn't always mean BITFIELDS
	if(bi->compression_field==3) {
		bi->size_of_headers_and_pal += 12; // BITFIELDS
	}

	bi->is_compressed = !((bi->compression_field==0) ||
		(bi->compression_field==3 && bi->bitcount>1));

	if(!de_good_image_dimensions(c, bi->width, bi->height)) {
		return 0;
	}

	// TODO: This needs work, to decide how to handle compressed images.
	// TODO: What about BI_BITFIELDS images?
	if(bi->compression_field==0) {
		// Try to figure out the true size of the resource, minus any padding.

		bi->rowspan = ((bi->bitcount*bi->width +31)/32)*4;
		bi->foreground_size = bi->rowspan * bi->height;
		de_dbg(c, "foreground size: %d", (int)bi->foreground_size);

		if(flags & DE_BMPINFO_ICO_FORMAT) {
			bi->mask_rowspan = ((bi->width +31)/32)*4;
			bi->mask_size = bi->mask_rowspan * bi->height;
			de_dbg(c, "mask size: %d", (int)bi->mask_size);
		}
		else {
			bi->mask_size = 0;
		}

		bi->total_size = bi->size_of_headers_and_pal + bi->foreground_size + bi->mask_size;
	}
	else {
		// Don't try to figure out the true size of compressed or other unusual images.
		bi->total_size = len;
	}

	return 1;
}