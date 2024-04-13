GF_EXPORT
void gf_isom_box_del(GF_Box *a)
{
	GF_List *child_boxes;
	const struct box_registry_entry *a_box_registry;
	if (!a) return;

	child_boxes	= a->child_boxes;
	a->child_boxes = NULL;

	a_box_registry = a->registry;
	if (!a_box_registry) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Delete invalid box type %s without registry\n", gf_4cc_to_str(a->type) ));
	} else {
		a_box_registry->del_fn(a);
	}
	//delete the other boxes after deleting the box for dumper case where all child boxes are stored in otherbox
	if (child_boxes) {
		gf_isom_box_array_del(child_boxes);
	}