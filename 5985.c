GF_Err txtc_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TextConfigBox *ptr = (GF_TextConfigBox*)s;
	if ((u32)ptr->size >= (u32)0xFFFFFFFF) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid size %llu in txtc box\n", ptr->size));
		return GF_ISOM_INVALID_FILE;
	}
	ptr->config = (char *)gf_malloc(sizeof(char)*((u32) ptr->size+1));
	if (!ptr->config) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->config, (u32) ptr->size);
	ptr->config[ptr->size] = 0;
	return GF_OK;
}