GF_Box *twrp_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextWrapBox, GF_ISOM_BOX_TYPE_TWRP);
	return (GF_Box *) tmp;
}