GF_Box *dlay_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextScrollDelayBox, GF_ISOM_BOX_TYPE_DLAY);
	return (GF_Box *) tmp;
}