GF_Err krok_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TextKaraokeBox*ptr = (GF_TextKaraokeBox*)s;

	ISOM_DECREASE_SIZE(ptr, 6)
	ptr->highlight_starttime = gf_bs_read_u32(bs);
	ptr->nb_entries = gf_bs_read_u16(bs);
	if (ptr->size / 8 < ptr->nb_entries)
		return GF_ISOM_INVALID_FILE;

	if (ptr->nb_entries) {
		u32 i;
		ptr->records = (KaraokeRecord*)gf_malloc(sizeof(KaraokeRecord)*ptr->nb_entries);
		if (!ptr->records) return GF_OUT_OF_MEM;
		for (i=0; i<ptr->nb_entries; i++) {
			ISOM_DECREASE_SIZE(ptr, 8)
			ptr->records[i].highlight_endtime = gf_bs_read_u32(bs);
			ptr->records[i].start_charoffset = gf_bs_read_u16(bs);
			ptr->records[i].end_charoffset = gf_bs_read_u16(bs);
		}
	}
	return GF_OK;
}