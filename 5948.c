GF_Box *stri_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SubTrackInformationBox, GF_ISOM_BOX_TYPE_STRI);
	return (GF_Box *)tmp;
}