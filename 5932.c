GF_Err sdtp_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_SampleDependencyTypeBox *ptr = (GF_SampleDependencyTypeBox*)s;

	/*out-of-order sdtp, assume no padding at the end*/
	if (!ptr->sampleCount) ptr->sampleCount = (u32) ptr->size;
	else if (ptr->sampleCount > (u32) ptr->size) return GF_ISOM_INVALID_FILE;

	ptr->sample_info = (u8 *) gf_malloc(sizeof(u8)*ptr->sampleCount);
	if (!ptr->sample_info) return GF_OUT_OF_MEM;
	ptr->sample_alloc = ptr->sampleCount;
	gf_bs_read_data(bs, (char*)ptr->sample_info, ptr->sampleCount);
	ISOM_DECREASE_SIZE(ptr, ptr->sampleCount);
	return GF_OK;
}