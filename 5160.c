GF_Err gppc_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_3GPPConfigBox *ptr = (GF_3GPPConfigBox *)s;
	if (ptr == NULL) return GF_BAD_PARAM;
	memset(&ptr->cfg, 0, sizeof(GF_3GPConfig));

	ISOM_DECREASE_SIZE(s, 5)
	ptr->cfg.vendor = gf_bs_read_u32(bs);
	ptr->cfg.decoder_version = gf_bs_read_u8(bs);

	switch (ptr->type) {
	case GF_ISOM_BOX_TYPE_D263:
		ISOM_DECREASE_SIZE(s, 2)
		ptr->cfg.H263_level = gf_bs_read_u8(bs);
		ptr->cfg.H263_profile = gf_bs_read_u8(bs);
		break;
	case GF_ISOM_BOX_TYPE_DAMR:
		ISOM_DECREASE_SIZE(s, 4)
		ptr->cfg.AMR_mode_set = gf_bs_read_u16(bs);
		ptr->cfg.AMR_mode_change_period = gf_bs_read_u8(bs);
		ptr->cfg.frames_per_sample = gf_bs_read_u8(bs);
		break;
	case GF_ISOM_BOX_TYPE_DEVC:
	case GF_ISOM_BOX_TYPE_DQCP:
	case GF_ISOM_BOX_TYPE_DSMV:
		ISOM_DECREASE_SIZE(s, 1)
		ptr->cfg.frames_per_sample = gf_bs_read_u8(bs);
		break;
	}
	return GF_OK;
}