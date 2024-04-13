GF_Err lsr1_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_LASeRSampleEntryBox *ptr = (GF_LASeRSampleEntryBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_data(bs, ptr->reserved, 6);
	gf_bs_write_u16(bs, ptr->dataReferenceIndex);
	return GF_OK;
}