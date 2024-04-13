GF_Err dims_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_DIMSSampleEntryBox *p = (GF_DIMSSampleEntryBox *)s;

	e = gf_isom_base_sample_entry_read((GF_SampleEntryBox *)p, bs);
	if (e) return e;

	ISOM_DECREASE_SIZE(p, 8);
	return gf_isom_box_array_read(s, bs);
}