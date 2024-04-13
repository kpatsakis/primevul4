GF_Err gitn_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GroupIdToNameBox *ptr = (GroupIdToNameBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, ptr->nb_entries);
	for (i=0; i<ptr->nb_entries; i++) {
		gf_bs_write_u32(bs, ptr->entries[i].group_id);
		if (ptr->entries[i].name) gf_bs_write_data(bs, ptr->entries[i].name, (u32)strlen(ptr->entries[i].name) );
		gf_bs_write_u8(bs, 0);
	}
	return GF_OK;
}