GF_Err gf_isom_box_array_size(GF_Box *parent, GF_List *list)
{
	GF_Err e;
	u32 count, i;
	if (! list) return GF_OK;

	count = gf_list_count(list);
	for (i = 0; i < count; i++) {
		GF_Box *a = (GF_Box *)gf_list_get(list, i);
		if (a) {
			e = gf_isom_box_size(a);
			if (e) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("ISOBMF: Error %s computing box %s size\n", gf_error_to_string(e), gf_4cc_to_str(a->type) ));
				return e;
			}
			parent->size += a->size;
		}
	}
	return GF_OK;
}