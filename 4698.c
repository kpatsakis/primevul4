int fmtutil_macbitmap_read_colortable(deark *c, dbuf *f,
	struct fmtutil_macbitmap_info *bi, i64 pos, i64 *bytes_used)
{
	i64 ct_id;
	u32 ct_flags;
	i64 ct_size;
	i64 k, z;
	u32 s[4];
	u8 cr, cg, cb;
	u32 clr;
	char tmps[64];

	*bytes_used = 0;
	de_dbg(c, "color table at %"I64_FMT, pos);
	de_dbg_indent(c, 1);

	ct_id = dbuf_getu32be(f, pos);
	ct_flags = (u32)dbuf_getu16be(f, pos+4); // a.k.a. transIndex
	ct_size = dbuf_getu16be(f, pos+6);
	bi->num_pal_entries = ct_size+1;
	de_dbg(c, "color table id=0x%08x, flags=0x%04x, colors=%d", (unsigned int)ct_id,
		(unsigned int)ct_flags, (int)bi->num_pal_entries);

	for(k=0; k<bi->num_pal_entries; k++) {
		for(z=0; z<4; z++) {
			s[z] = (u32)dbuf_getu16be(f, pos+8+8*k+2*z);
		}
		cr = (u8)(s[1]>>8);
		cg = (u8)(s[2]>>8);
		cb = (u8)(s[3]>>8);
		clr = DE_MAKE_RGB(cr,cg,cb);
		de_snprintf(tmps, sizeof(tmps), "(%5d,%5d,%5d,idx=%3d) "DE_CHAR_RIGHTARROW" ",
			(int)s[1], (int)s[2], (int)s[3], (int)s[0]);
		de_dbg_pal_entry2(c, k, clr, tmps, NULL, NULL);

		// Some files don't have the palette indices set. Most PICT decoders ignore
		// the indices if the "device" flag of ct_flags is set, and that seems to
		// work (though it's not clearly documented).
		if(ct_flags & 0x8000U) {
			s[0] = (u32)k;
		}

		if(s[0]<=255) {
			bi->pal[s[0]] = clr;
		}
	}

	de_dbg_indent(c, -1);
	*bytes_used = 8 + 8*bi->num_pal_entries;
	return 1;
}