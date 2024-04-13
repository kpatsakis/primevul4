GF_Box *hlit_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextHighlightBox, GF_ISOM_BOX_TYPE_HLIT);
	return (GF_Box *) tmp;
}