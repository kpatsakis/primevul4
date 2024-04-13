GF_Err gf_isom_set_write_callback(GF_ISOFile *mov,
 			GF_Err (*on_block_out)(void *cbk, u8 *data, u32 block_size),
			GF_Err (*on_block_patch)(void *usr_data, u8 *block, u32 block_size, u64 block_offset, Bool is_insert),
 			void *usr_data,
 			u32 block_size)
{
#ifndef GPAC_DISABLE_ISOM_WRITE
	if (mov->finalName && !strcmp(mov->finalName, "_gpac_isobmff_redirect")) {}
	else if (mov->fileName && !strcmp(mov->fileName, "_gpac_isobmff_redirect")) {}
	else return GF_BAD_PARAM;
	mov->on_block_out = on_block_out;
	mov->on_block_patch = on_block_patch;
	mov->on_block_out_usr_data = usr_data;
	mov->on_block_out_block_size = block_size;
	return GF_OK;
#else
	return GF_NOT_SUPPORTED;
#endif
}