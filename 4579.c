
Bool gf_box_valid_in_parent(GF_Box *a, const char *parent_4cc)
{
	if (!a || !a->registry || !a->registry->parents_4cc) return GF_FALSE;
	if (strstr(a->registry->parents_4cc, parent_4cc) != NULL) return GF_TRUE;
	return GF_FALSE;