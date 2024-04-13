GF_Box *subs_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_SubSampleInformationBox, GF_ISOM_BOX_TYPE_SUBS);
	tmp->Samples = gf_list_new();
	return (GF_Box *)tmp;
}