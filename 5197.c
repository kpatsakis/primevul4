GF_Box *dims_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_DIMSSampleEntryBox, GF_ISOM_BOX_TYPE_DIMS);
	gf_isom_sample_entry_init((GF_SampleEntryBox *)tmp);
	return (GF_Box*)tmp;
}