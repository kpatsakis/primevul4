void gf_isom_box_array_del_parent(GF_List **child_boxes, GF_List *boxlist)
{
	if (!boxlist) return;
	gf_isom_box_array_reset_parent(child_boxes, boxlist);
	gf_list_del(boxlist);
}