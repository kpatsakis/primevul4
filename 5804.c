static void isor_update_cenc_info(ISOMChannel *ch, Bool for_item)
{
	GF_Err e;
	Bool Is_Encrypted;
	u32 out_size;
	u8 crypt_byte_block, skip_byte_block;
	u8 piff_info[20];
	u8 *key_info = NULL;
	u32 key_info_size = 0;
	u8 item_mkey = 0;

	//this will be skipped anyways, don't fectch ...
	if (ch->owner->stsd && (ch->last_sample_desc_index != ch->owner->stsd) && ch->sample) {
		return;
	}


	out_size = ch->sai_alloc_size;
	if (for_item) {
		u32 aux_info_param=0;
		e = gf_isom_extract_meta_item_get_cenc_info(ch->owner->mov, GF_TRUE, 0, ch->item_id, &Is_Encrypted, &skip_byte_block, &crypt_byte_block, (const u8 **) &key_info, &key_info_size, &aux_info_param, &ch->sai_buffer, &out_size, &ch->sai_alloc_size);

		/*The ienc property is always exposed as a multiple key info in GPAC
		However the type of SAI may be single-key (aux_info_param==0) or multiple-key (aux_info_param==1) for the same ienc used
		We therefore temporary force the key info type to single key if v0 SAI CENC are used
		Note that this is thread safe as this filter is the only one using the opened file
		*/
		if (aux_info_param==0) {
			item_mkey = key_info[0];
		}
	} else {
		e = gf_isom_get_sample_cenc_info(ch->owner->mov, ch->track, ch->sample_num, &Is_Encrypted, &crypt_byte_block, &skip_byte_block, (const u8 **) &key_info, &key_info_size);
	}
	if (!key_info) {
		piff_info[0] = 0;
		piff_info[1] = 0;
		piff_info[2] = 0;
		piff_info[3] = key_info_size;
		memset(piff_info + 4, 0, 16);
		key_info_size = 20;
		key_info = (u8 *) piff_info;
	}


	if (!for_item && (e==GF_OK) && Is_Encrypted) {
		e = gf_isom_cenc_get_sample_aux_info(ch->owner->mov, ch->track, ch->sample_num, ch->last_sample_desc_index, NULL, &ch->sai_buffer, &out_size);
	}

	if (out_size > ch->sai_alloc_size) ch->sai_alloc_size = out_size;
	ch->sai_buffer_size = out_size;

	if (e) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[IsoMedia] Failed to fetch CENC auxiliary info for %s %d: %s\n", for_item ? "item" : "track", for_item ? ch->item_id : ch->track, gf_error_to_string(e) ));
		return;
	}

	ch->pck_encrypted = Is_Encrypted;
	ch->cenc_ki = NULL;

	/*notify change of IV/KID only when packet is encrypted
	1- these info are ignored when packet is not encrypted
	2- this allows us to define the initial CENC state for multi-stsd cases*/
	if (Is_Encrypted) {
		u32 ki_crc;

		if ((ch->crypt_byte_block != crypt_byte_block) || (ch->skip_byte_block != skip_byte_block)) {
			ch->crypt_byte_block = crypt_byte_block;
			ch->skip_byte_block = skip_byte_block;

			gf_filter_pid_set_property(ch->pid, GF_PROP_PID_CENC_PATTERN, &PROP_FRAC_INT(ch->skip_byte_block, ch->crypt_byte_block) );
		}
		if (item_mkey)
			key_info[0] = 0;

		ki_crc = gf_crc_32(key_info, key_info_size);
		if (ch->key_info_crc != ki_crc) {
			ch->key_info_crc = ki_crc;
			gf_filter_pid_set_property(ch->pid, GF_PROP_PID_CENC_KEY_INFO, &PROP_DATA((u8 *)key_info, key_info_size) );
		}

		if (item_mkey)
			key_info[0] = item_mkey;

		ch->cenc_ki = gf_filter_pid_get_property(ch->pid, GF_PROP_PID_CENC_KEY_INFO);
	}
}