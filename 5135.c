GF_Box *gppc_box_new()
{
	//default type is amr but overwritten by box constructor
	ISOM_DECL_BOX_ALLOC(GF_3GPPConfigBox, GF_ISOM_BOX_TYPE_DAMR);
	return (GF_Box *)tmp;
}