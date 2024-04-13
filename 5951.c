GF_Box *txtc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TextConfigBox, GF_ISOM_BOX_TYPE_TXTC);
	return (GF_Box *)tmp;
}