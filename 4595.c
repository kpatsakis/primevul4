
static GF_Err gf_isom_box_size_listing(GF_Box *a)
{
	if (!a) return GF_BAD_PARAM;
	if (!a->registry) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Size invalid box type %s without registry\n", gf_4cc_to_str(a->type) ));
		return GF_ISOM_INVALID_FILE;
	}
	a->size = 8;

	if (a->type == GF_ISOM_BOX_TYPE_UUID) {
		a->size += 16;
	}
	//the large size is handled during write, cause at this stage we don't know the size
	if (a->registry->max_version_plus_one) {
		a->size += 4;
	}
	return a->registry->size_fn(a);