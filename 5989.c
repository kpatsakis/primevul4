GF_Box *dmlp_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrueHDConfigBox, GF_ISOM_BOX_TYPE_DMLP);
	return (GF_Box *)tmp;
}