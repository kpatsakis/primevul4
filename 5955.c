GF_Err dfla_box_size(GF_Box *s)
{
	GF_FLACConfigBox *ptr = (GF_FLACConfigBox *) s;
	ptr->size += ptr->dataSize;
	return GF_OK;
}