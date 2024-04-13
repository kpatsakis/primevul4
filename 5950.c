GF_Box *segr_box_new()
{
	ISOM_DECL_BOX_ALLOC(FDSessionGroupBox, GF_ISOM_BOX_TYPE_SEGR);
	return (GF_Box *)tmp;
}