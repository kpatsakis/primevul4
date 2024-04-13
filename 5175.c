GF_Box *text_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextSampleEntryBox, GF_ISOM_BOX_TYPE_TEXT);
	gf_isom_sample_entry_init((GF_SampleEntryBox *)tmp);
	return (GF_Box *) tmp;
}