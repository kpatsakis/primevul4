GF_Err reftype_box_size(GF_Box *s)
{
	GF_TrackReferenceTypeBox *ptr = (GF_TrackReferenceTypeBox *)s;
	if (ptr->trackIDCount)
		ptr->size += (ptr->trackIDCount * sizeof(u32));
	return GF_OK;
}