GF_Box *tsel_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackSelectionBox, GF_ISOM_BOX_TYPE_TSEL);
	return (GF_Box *)tmp;
}