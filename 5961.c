GF_Err rvcc_box_size(GF_Box *s)
{
	GF_RVCConfigurationBox *ptr = (GF_RVCConfigurationBox *)s;
	ptr->size += 2;
	if (! ptr->predefined_rvc_config) ptr->size += 2;
	return GF_OK;
}