
void gf_isom_box_freeze_order(GF_Box *box)
{
	u32 i=0;
	GF_Box *child;
	if (!box) return;
	box->internal_flags |= GF_ISOM_ORDER_FREEZE;

	while ((child = gf_list_enum(box->child_boxes, &i))) {
		gf_isom_box_freeze_order(child);
	}
