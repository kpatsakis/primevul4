GF_Err krok_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_TextKaraokeBox*ptr = (GF_TextKaraokeBox*)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->highlight_starttime);
	gf_bs_write_u16(bs, ptr->nb_entries);
	for (i=0; i<ptr->nb_entries; i++) {
		gf_bs_write_u32(bs, ptr->records[i].highlight_endtime);
		gf_bs_write_u16(bs, ptr->records[i].start_charoffset);
		gf_bs_write_u16(bs, ptr->records[i].end_charoffset);
	}
	return GF_OK;
}