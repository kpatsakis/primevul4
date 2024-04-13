GF_Err diST_box_size(GF_Box *s)
{
	GF_DIMSScriptTypesBox *p = (GF_DIMSScriptTypesBox *)s;
	s->size += p->content_script_types ? (strlen(p->content_script_types)+1) : 1;
	return GF_OK;
}