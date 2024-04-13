GF_Err fecr_box_size(GF_Box *s)
{
	FECReservoirBox *ptr = (FECReservoirBox *)s;
	ptr->size += (ptr->version ? 4 : 2) +  ptr->nb_entries * (ptr->version ? 8 : 6);
	return GF_OK;
}