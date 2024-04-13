void text_box_del(GF_Box *s)
{
	GF_TextSampleEntryBox *ptr = (GF_TextSampleEntryBox*)s;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)s);

	if (ptr->textName)
		gf_free(ptr->textName);
	gf_free(ptr);
}