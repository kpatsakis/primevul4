GF_Err tsel_box_read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_TrackSelectionBox *ptr = (GF_TrackSelectionBox *) s;

	ISOM_DECREASE_SIZE(ptr, 4);
	ptr->switchGroup = gf_bs_read_u32(bs);

	if (ptr->size % 4) return GF_ISOM_INVALID_FILE;
	ptr->attributeListCount = (u32)ptr->size/4;
	ptr->attributeList = gf_malloc(ptr->attributeListCount*sizeof(u32));
	if (ptr->attributeList == NULL) return GF_OUT_OF_MEM;

	for (i=0; i< ptr->attributeListCount; i++) {
		ptr->attributeList[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;
}