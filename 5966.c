GF_Box *fiin_box_new()
{
	ISOM_DECL_BOX_ALLOC(FDItemInformationBox, GF_ISOM_BOX_TYPE_FIIN);
	return (GF_Box *)tmp;
}