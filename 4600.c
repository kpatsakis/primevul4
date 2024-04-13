
GF_Box *gf_isom_box_new_parent(GF_List **parent, u32 code)
{
	GF_Box *b = gf_isom_box_new(code);
	if (!b) return NULL;
	if (! (*parent) ) (*parent)  = gf_list_new();
	gf_list_add(*parent, b);
	return b;