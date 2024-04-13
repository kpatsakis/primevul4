
GF_Box *gf_isom_box_find_child(GF_List *children, u32 code)
{
	u32 i, count;
	if (!children) return NULL;
	count = gf_list_count(children);
	for (i=0; i<count; i++) {
		GF_Box *c = gf_list_get(children, i);
		if (c->type==code) return c;

		if (c->type==GF_ISOM_BOX_TYPE_UNKNOWN) {
			if (((GF_UnknownBox*)c)->original_4cc==code)
				return c;
		}
		if (c->type==GF_ISOM_BOX_TYPE_UUID) {
			if (((GF_UUIDBox*)c)->internal_4cc==code)
				return c;
		}
	}
	return NULL;