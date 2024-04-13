void fmtutil_read_atari_palette(deark *c, dbuf *f, i64 pos,
	de_color *dstpal, i64 ncolors_to_read, i64 ncolors_used, unsigned int flags)
{
	i64 i;
	unsigned int n;
	int pal_bits = 0; // 9, 12, or 15. 0 = not yet determined
	u8 cr, cg, cb;
	u8 cr1, cg1, cb1;
	char cbuf[32];
	char tmps[64];
	const char *s;

	s = de_get_ext_option(c, "atari:palbits");
	if(s) {
		pal_bits = de_atoi(s);
	}

	if(pal_bits==0 && (flags&DE_FLAG_ATARI_15BIT_PAL)) {
		pal_bits = 15;
	}

	if(pal_bits==0) {
		// Pre-scan the palette, and try to guess whether Atari STE-style 12-bit
		// colors are used, instead of the usual 9-bit colors.
		// I don't know the best way to do this. Sometimes the 4th bit in each
		// nibble is used for extra color detail, and sometimes it just seems to
		// contain garbage. Maybe the logic should also depend on the file
		// format, or the number of colors.
		int bit_3_used = 0;
		int nibble_3_used = 0;

		for(i=0; i<ncolors_to_read; i++) {
			n = (unsigned int)dbuf_getu16be(f, pos + i*2);
			if(n&0xf000) {
				nibble_3_used = 1;
			}
			if(n&0x0888) {
				bit_3_used = 1;
			}
		}

		if(bit_3_used && !nibble_3_used) {
			de_dbg(c, "12-bit palette colors detected");
			pal_bits = 12;
		}
	}

	if(pal_bits<12) { // Default to 9 if <12
		pal_bits = 9;
	}
	else if(pal_bits<15) {
		pal_bits = 12;
	}
	else {
		pal_bits = 15;
	}

	for(i=0; i<ncolors_to_read; i++) {
		n = (unsigned int)dbuf_getu16be(f, pos + 2*i);

		if(pal_bits==15) {
			cr1 = (u8)((n>>6)&0x1c);
			if(n&0x0800) cr1+=2;
			if(n&0x8000) cr1++;
			cg1 = (u8)((n>>2)&0x1c);
			if(n&0x0080) cg1+=2;
			if(n&0x4000) cg1++;
			cb1 = (u8)((n<<2)&0x1c);
			if(n&0x0008) cb1+=2;
			if(n&0x2000) cb1++;
			cr = de_scale_n_to_255(31, cr1);
			cg = de_scale_n_to_255(31, cg1);
			cb = de_scale_n_to_255(31, cb1);
			de_snprintf(cbuf, sizeof(cbuf), "%2d,%2d,%2d",
				(int)cr1, (int)cg1, (int)cb1);
		}
		else if(pal_bits==12) {
			cr1 = (u8)((n>>7)&0x0e);
			if(n&0x800) cr1++;
			cg1 = (u8)((n>>3)&0x0e);
			if(n&0x080) cg1++;
			cb1 = (u8)((n<<1)&0x0e);
			if(n&0x008) cb1++;
			cr = scale_15_to_255(cr1);
			cg = scale_15_to_255(cg1);
			cb = scale_15_to_255(cb1);
			de_snprintf(cbuf, sizeof(cbuf), "%2d,%2d,%2d",
				(int)cr1, (int)cg1, (int)cb1);
		}
		else {
			cr1 = (u8)((n>>8)&0x07);
			cg1 = (u8)((n>>4)&0x07);
			cb1 = (u8)(n&0x07);
			cr = scale_7_to_255(cr1);
			cg = scale_7_to_255(cg1);
			cb = scale_7_to_255(cb1);
			de_snprintf(cbuf, sizeof(cbuf), "%d,%d,%d",
				(int)cr1, (int)cg1, (int)cb1);
		}

		dstpal[i] = DE_MAKE_RGB(cr, cg, cb);
		de_snprintf(tmps, sizeof(tmps), "0x%04x (%s) "DE_CHAR_RIGHTARROW" ", n, cbuf);
		de_dbg_pal_entry2(c, i, dstpal[i], tmps, NULL,
			(i>=ncolors_used)?" [unused]":"");
	}
}