GF_EXPORT
u32 gf_isom_get_supported_box_type(u32 idx)
{
	return box_registry[idx].box_4cc;