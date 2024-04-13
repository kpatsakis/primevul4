GF_EXPORT
GF_Err gf_isom_box_write(GF_Box *a, GF_BitStream *bs)
{
	GF_Err e;
	u64 pos = gf_bs_get_position(bs);
	if (!a) return GF_BAD_PARAM;
	//box has been disabled, do not write
	if (!a->size) return GF_OK;
	
	if (a->registry->disabled) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[iso file] Box %s disabled registry, skip write\n", gf_4cc_to_str(a->type)));
		return GF_OK;
	}

	GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[iso file] Box %s size %d write\n", gf_4cc_to_str(a->type), a->size));
	e = gf_isom_box_write_listing(a, bs);
	if (e) return e;
	if (a->child_boxes) {
		e = gf_isom_box_array_write(a, a->child_boxes, bs);
	}
	pos = gf_bs_get_position(bs) - pos;
	if (pos != a->size) {
		if (a->type != GF_ISOM_BOX_TYPE_MDAT) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Box %s wrote "LLU" bytes but size is "LLU"\n", gf_4cc_to_str(a->type), pos, a->size ));
		}
	}
	return e;