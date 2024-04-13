void gf_isom_box_array_reset(GF_List *boxlist)
{
	u32 count, i;
	if (!boxlist) return;
	count = gf_list_count(boxlist);
	for (i = 0; i < count; i++) {
		GF_Box *a = (GF_Box *)gf_list_get(boxlist, i);
		if (a) gf_isom_box_del(a);
	}
}