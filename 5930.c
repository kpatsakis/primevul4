void stsg_box_del(GF_Box *s)
{
	GF_SubTrackSampleGroupBox *ptr = (GF_SubTrackSampleGroupBox *)s;
	if (ptr == NULL) return;
	if (ptr->group_description_index) gf_free(ptr->group_description_index);
	gf_free(ptr);
}