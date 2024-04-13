void xtra_box_del(GF_Box *s)
{
	GF_XtraBox *ptr = (GF_XtraBox *)s;
	while (gf_list_count(ptr->tags)) {
		GF_XtraTag *tag = gf_list_pop_back(ptr->tags);
		if (tag->name) gf_free(tag->name);
		if (tag->prop_value) gf_free(tag->prop_value);
		gf_free(tag);
	}
	gf_list_del(ptr->tags);
	gf_free(s);
}