GF_Box *saio_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SampleAuxiliaryInfoOffsetBox, GF_ISOM_BOX_TYPE_SAIO);
	return (GF_Box *)tmp;
}