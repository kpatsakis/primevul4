GF_EXPORT
GF_Err gf_isom_box_size(GF_Box *a)
{
	GF_Err e;
	if (!a) return GF_BAD_PARAM;
	if (a->registry->disabled) {
		a->size = 0;
		return GF_OK;
	}
	e = gf_isom_box_size_listing(a);
	if (e) return e;
	//box size set to 0 (not even a header), abort traversal
	if (!a->size) return GF_OK;

	if (a->child_boxes) {
		e = gf_isom_box_array_size(a, a->child_boxes);
		if (e) return e;
	}
	return GF_OK;