GF_Err stsz_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_SampleSizeBox *ptr = (GF_SampleSizeBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	//in both versions this is still valid
	if (ptr->type == GF_ISOM_BOX_TYPE_STSZ) {
		gf_bs_write_u32(bs, ptr->sampleSize);
	} else {
		gf_bs_write_u24(bs, 0);
		gf_bs_write_u8(bs, ptr->sampleSize);
	}
	gf_bs_write_u32(bs, ptr->sampleCount);

	if (ptr->type == GF_ISOM_BOX_TYPE_STSZ) {
		if (ptr->sampleSize) return GF_OK;
		for (i = 0; i < ptr->sampleCount; i++) {
			gf_bs_write_u32(bs, ptr->sizes ? ptr->sizes[i] : 0);
		}
	} else {
		if (!ptr->sizes) return GF_ISOM_INVALID_FILE;
		for (i = 0; i < ptr->sampleCount; ) {
			switch (ptr->sampleSize) {
			case 4:
				gf_bs_write_int(bs, ptr->sizes[i], 4);
				if (i+1 < ptr->sampleCount) {
					gf_bs_write_int(bs, ptr->sizes[i+1], 4);
				} else {
					//0 padding in odd sample count
					gf_bs_write_int(bs, 0, 4);
				}
				i += 2;
				break;
			default:
				gf_bs_write_int(bs, ptr->sizes[i], ptr->sampleSize);
				i += 1;
				break;
			}
		}
	}
	return GF_OK;
}