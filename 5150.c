GF_Err tsel_box_size(GF_Box *s)
{
	GF_TrackSelectionBox *ptr = (GF_TrackSelectionBox *) s;
	ptr->size += 4 + (4*ptr->attributeListCount);
	return GF_OK;
}