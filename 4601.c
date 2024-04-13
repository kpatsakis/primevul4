GF_EXPORT
u32 gf_isom_get_num_supported_boxes()
{
	return sizeof(box_registry) / sizeof(struct box_registry_entry);