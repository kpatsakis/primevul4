GF_Box *ftab_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_FontTableBox, GF_ISOM_BOX_TYPE_FTAB);
	return (GF_Box *) tmp;
}