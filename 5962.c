GF_Err pcmC_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_PCMConfigBox *ptr = (GF_PCMConfigBox *) s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u8(bs, ptr->format_flags);
	gf_bs_write_u8(bs, ptr->PCM_sample_size);
	return GF_OK;
}