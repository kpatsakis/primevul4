GF_Err dvcC_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_DOVIConfigurationBox *ptr = (GF_DOVIConfigurationBox *)s;

	//GF_DOVIDecoderConfigurationRecord
	ISOM_DECREASE_SIZE(ptr, 24)
	ptr->DOVIConfig.dv_version_major = gf_bs_read_u8(bs);
	ptr->DOVIConfig.dv_version_minor = gf_bs_read_u8(bs);
	ptr->DOVIConfig.dv_profile = gf_bs_read_int(bs, 7);
	ptr->DOVIConfig.dv_level = gf_bs_read_int(bs, 6);
	ptr->DOVIConfig.rpu_present_flag = gf_bs_read_int(bs, 1);
	ptr->DOVIConfig.el_present_flag = gf_bs_read_int(bs, 1);
	ptr->DOVIConfig.bl_present_flag = gf_bs_read_int(bs, 1);
	ptr->DOVIConfig.dv_bl_signal_compatibility_id = gf_bs_read_int(bs, 4);
	if (gf_bs_read_int(bs, 28) != 0)
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] dvcC reserved bits are not zero\n"));

	for (i = 0; i < 4; i++) {
		if (gf_bs_read_u32(bs) != 0) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] dvcC reserved bits are not zero\n"));
		}
	}
	if (ptr->DOVIConfig.dv_profile==8) {
		if (!ptr->DOVIConfig.dv_bl_signal_compatibility_id || (ptr->DOVIConfig.dv_bl_signal_compatibility_id>2) ) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] dvcC profile 8 but compatibility ID %d is not 1 or 2, patching to 2\n", ptr->DOVIConfig.dv_bl_signal_compatibility_id));
			ptr->DOVIConfig.dv_bl_signal_compatibility_id = 2;
		}
	}
	return GF_OK;
}