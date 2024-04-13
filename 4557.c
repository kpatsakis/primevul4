
void gf_isom_check_position_list(GF_Box *s, GF_List *childlist, u32 *pos)
{
	u32 i, count;
	if (!s || (s->internal_flags & GF_ISOM_ORDER_FREEZE))
		return;
	count = gf_list_count(childlist);
	for (i=0; i<count; i++) {
		GF_Box *child = gf_list_get(childlist, i);
		gf_isom_check_position(s, child, pos);
	}