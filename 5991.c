GF_Box *dOps_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_OpusSpecificBox, GF_ISOM_BOX_TYPE_DOPS);
	return (GF_Box *)tmp;
}