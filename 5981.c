GF_Err mhac_box_size(GF_Box *s)
{
	GF_MHAConfigBox *ptr = (GF_MHAConfigBox *) s;
	s->size += 5;
	if (ptr->mha_config_size && ptr->mha_config) s->size += ptr->mha_config_size;
	return GF_OK;
}