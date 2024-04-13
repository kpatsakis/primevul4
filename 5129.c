GF_Err gppc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_3GPPConfigBox *ptr = (GF_3GPPConfigBox *)s;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->cfg.vendor);
	gf_bs_write_u8(bs, ptr->cfg.decoder_version);
	switch (ptr->cfg.type) {
	case GF_ISOM_SUBTYPE_3GP_H263:
		gf_bs_write_u8(bs, ptr->cfg.H263_level);
		gf_bs_write_u8(bs, ptr->cfg.H263_profile);
		break;
	case GF_ISOM_SUBTYPE_3GP_AMR:
	case GF_ISOM_SUBTYPE_3GP_AMR_WB:
		gf_bs_write_u16(bs, ptr->cfg.AMR_mode_set);
		gf_bs_write_u8(bs, ptr->cfg.AMR_mode_change_period);
		gf_bs_write_u8(bs, ptr->cfg.frames_per_sample);
		break;
	case GF_ISOM_SUBTYPE_3GP_EVRC:
	case GF_ISOM_SUBTYPE_3GP_QCELP:
	case GF_ISOM_SUBTYPE_3GP_SMV:
		gf_bs_write_u8(bs, ptr->cfg.frames_per_sample);
		break;
	}
	return GF_OK;
}