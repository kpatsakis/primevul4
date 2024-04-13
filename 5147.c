GF_Box *hclr_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextHighlightColorBox, GF_ISOM_BOX_TYPE_HCLR);
	return (GF_Box *) tmp;
}