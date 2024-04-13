GF_Box *prhd_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ProjectionHeaderBox, GF_ISOM_BOX_TYPE_PRHD);
	return (GF_Box *)tmp;
}