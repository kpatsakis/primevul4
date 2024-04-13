
void gf_isom_registry_disable(u32 boxCode, Bool disable)
{
	u32 i=0, count = gf_isom_get_num_supported_boxes();
	for (i=1; i<count; i++) {
		if (box_registry[i].box_4cc==boxCode) {
			box_registry[i].disabled = disable;
			return;
		}
	}