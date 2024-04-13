GF_Err trik_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_TrickPlayBox *ptr = (GF_TrickPlayBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	for (i=0; i < ptr->entry_count; i++ ) {
		gf_bs_write_int(bs, ptr->entries[i].pic_type, 2);
		gf_bs_write_int(bs, ptr->entries[i].dependency_level, 6);
	}
	return GF_OK;
}