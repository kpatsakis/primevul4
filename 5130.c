GF_Err tsel_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_TrackSelectionBox *ptr = (GF_TrackSelectionBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs,ptr->switchGroup);

	for (i = 0; i < ptr->attributeListCount; i++ ) {
		gf_bs_write_u32(bs, ptr->attributeList[i]);
	}

	return GF_OK;
}