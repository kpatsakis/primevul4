void tsel_box_del(GF_Box *s)
{
	GF_TrackSelectionBox *ptr;
	ptr = (GF_TrackSelectionBox *) s;
	if (ptr == NULL) return;
	if (ptr->attributeList) gf_free(ptr->attributeList);
	gf_free(ptr);
}