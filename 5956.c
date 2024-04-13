GF_Err fecr_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	FECReservoirBox *ptr = (FECReservoirBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_int(bs, ptr->nb_entries, ptr->version ? 32 : 16);
	for (i=0; i<ptr->nb_entries; i++) {
		gf_bs_write_int(bs, ptr->entries[i].item_id, ptr->version ? 32 : 16);
		gf_bs_write_u32(bs, ptr->entries[i].symbol_count);
	}
	return GF_OK;
}