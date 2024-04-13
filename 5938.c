void trgr_box_del(GF_Box *s)
{
	GF_TrackGroupBox *ptr = (GF_TrackGroupBox *)s;
	if (ptr == NULL) return;
	gf_list_del(ptr->groups);
	gf_free(ptr);
}