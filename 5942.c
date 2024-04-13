GF_Err lsrc_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_LASERConfigurationBox *ptr = (GF_LASERConfigurationBox *)s;
	ptr->hdr_size = (u32) ptr->size;
	ptr->hdr = gf_malloc(sizeof(char)*ptr->hdr_size);
	if (!ptr->hdr) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->hdr, ptr->hdr_size);
	return GF_OK;
}