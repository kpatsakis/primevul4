void gf_isom_box_array_del(GF_List *boxlist)
{
	gf_isom_box_array_reset(boxlist);
	gf_list_del(boxlist);
}