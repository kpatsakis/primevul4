GF_Err dims_box_size(GF_Box *s)
{
	u32 pos = 0;
	GF_DIMSSampleEntryBox *p = (GF_DIMSSampleEntryBox *)s;
	s->size += 8;
	gf_isom_check_position(s, (GF_Box *) p->config, &pos);
	gf_isom_check_position(s, (GF_Box *) p->scripts, &pos);
	return GF_OK;
}