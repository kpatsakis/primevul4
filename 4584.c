
void gf_isom_box_del_parent(GF_List **child_boxes, GF_Box*b)
{
	if (child_boxes) {
		gf_list_del_item(*child_boxes, b);
		if (!gf_list_count(*child_boxes)) {
			gf_list_del(*child_boxes);
			*child_boxes = NULL;
		}
	}
	gf_isom_box_del(b);