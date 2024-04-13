
Bool gf_isom_box_is_file_level(GF_Box *s)
{
	if (!s || !s->registry) return GF_FALSE;
	if (strstr(s->registry->parents_4cc, "file")!= NULL) return GF_TRUE;
	if (strstr(s->registry->parents_4cc, "*")!= NULL) return GF_TRUE;
	return GF_FALSE;