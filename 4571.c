
Bool gf_isom_box_check_unique(GF_List *children, GF_Box *a)
{
	u32 i, count;
	if (!children) return GF_TRUE;
	count = gf_list_count(children);
	for (i=0; i<count; i++) {
		GF_Box *c = gf_list_get(children, i);
		if (c==a) continue;
		if (c->type==a->type) return GF_FALSE;
	}
	return GF_TRUE;