GF_Err fiin_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	FDItemInformationBox *ptr = (FDItemInformationBox *) s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u16(bs, gf_list_count(ptr->partition_entries) );
	return GF_OK;
}