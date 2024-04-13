GF_Err saiz_box_size(GF_Box *s)
{
	GF_SampleAuxiliaryInfoSizeBox *ptr = (GF_SampleAuxiliaryInfoSizeBox*)s;

	if (ptr->aux_info_type || ptr->aux_info_type_parameter) {
		ptr->flags |= 1;
	}
	if (ptr->flags & 1) ptr->size += 8;
	ptr->size += 5;
	if (ptr->default_sample_info_size==0)  ptr->size += ptr->sample_count;
	return GF_OK;
}