GF_Box *trep_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackExtensionPropertiesBox, GF_ISOM_BOX_TYPE_TREP);
	tmp->child_boxes = gf_list_new();
	return (GF_Box *)tmp;
}