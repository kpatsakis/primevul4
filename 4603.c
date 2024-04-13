GF_EXPORT
GF_Box *gf_isom_box_new(u32 boxType)
{
	return gf_isom_box_new_ex(boxType, 0, 0, GF_FALSE);