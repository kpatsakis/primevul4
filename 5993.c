GF_Err fpar_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_Err e;
	FilePartitionBox *ptr = (FilePartitionBox *)s;

	ISOM_DECREASE_SIZE(ptr, ((ptr->version ? 4 : 2) + 12) );
	ptr->itemID = gf_bs_read_int(bs, ptr->version ? 32 : 16);
	ptr->packet_payload_size = gf_bs_read_u16(bs);
	gf_bs_read_u8(bs);
	ptr->FEC_encoding_ID = gf_bs_read_u8(bs);
	ptr->FEC_instance_ID = gf_bs_read_u16(bs);
	ptr->max_source_block_length = gf_bs_read_u16(bs);
	ptr->encoding_symbol_length = gf_bs_read_u16(bs);
	ptr->max_number_of_encoding_symbols = gf_bs_read_u16(bs);

	e = gf_isom_read_null_terminated_string(s, bs, ptr->size, &ptr->scheme_specific_info);
	if (e) return e;

	ISOM_DECREASE_SIZE(ptr, (ptr->version ? 4 : 2) );
	ptr->nb_entries = gf_bs_read_int(bs, ptr->version ? 32 : 16);
	if (ptr->nb_entries > ptr->size / 6 || (u64)ptr->nb_entries > (u64)SIZE_MAX/sizeof(FilePartitionEntry))
		return GF_ISOM_INVALID_FILE;

	ISOM_DECREASE_SIZE(ptr, ptr->nb_entries * 6 );
	GF_SAFE_ALLOC_N(ptr->entries, ptr->nb_entries, FilePartitionEntry);
	if (!ptr->entries) return GF_OUT_OF_MEM;

	for (i=0;i < ptr->nb_entries; i++) {
		ptr->entries[i].block_count = gf_bs_read_u16(bs);
		ptr->entries[i].block_size = gf_bs_read_u32(bs);
	}
	return GF_OK;
}