GF_Box *styl_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextStyleBox, GF_ISOM_BOX_TYPE_STYL);
	return (GF_Box *) tmp;
}