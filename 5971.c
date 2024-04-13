GF_Err mhac_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_MHAConfigBox *ptr = (GF_MHAConfigBox *) s;

	ISOM_DECREASE_SIZE(s, 5)
	ptr->configuration_version = gf_bs_read_u8(bs);
	ptr->mha_pl_indication = gf_bs_read_u8(bs);
	ptr->reference_channel_layout = gf_bs_read_u8(bs);
	ptr->mha_config_size = gf_bs_read_u16(bs);
	if (ptr->mha_config_size) {
		ISOM_DECREASE_SIZE(s, ptr->mha_config_size)

		ptr->mha_config = gf_malloc(sizeof(char)*ptr->mha_config_size);
		if (!ptr->mha_config) return GF_OUT_OF_MEM;

		gf_bs_read_data(bs, ptr->mha_config, ptr->mha_config_size);
	}
	return GF_OK;
}