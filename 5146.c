GF_Err dims_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_DIMSSampleEntryBox *p = (GF_DIMSSampleEntryBox *)s;
	GF_Err e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_data(bs, p->reserved, 6);
	gf_bs_write_u16(bs, p->dataReferenceIndex);
	return GF_OK;
}