GF_Box *xtra_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_XtraBox, GF_ISOM_BOX_TYPE_XTRA);
	tmp->tags = gf_list_new();
	return (GF_Box *)tmp;
}