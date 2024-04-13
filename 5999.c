void mvcg_box_del(GF_Box *s)
{
	GF_MultiviewGroupBox *ptr = (GF_MultiviewGroupBox *) s;
	if (ptr->entries) gf_free(ptr->entries);
	gf_free(ptr);
}