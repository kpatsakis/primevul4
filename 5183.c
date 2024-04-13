void diST_box_del(GF_Box *s)
{
	GF_DIMSScriptTypesBox *p = (GF_DIMSScriptTypesBox *)s;
	if (p->content_script_types) gf_free(p->content_script_types);
	gf_free(p);
}