GF_Box *tbox_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextBoxBox, GF_ISOM_BOX_TYPE_TBOX);
	return (GF_Box *) tmp;
}