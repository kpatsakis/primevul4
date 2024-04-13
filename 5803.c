void isor_set_crypt_config(ISOMChannel *ch)
{
	GF_ISOFile *mov = ch->owner->mov;
	u32 track = ch->track;
	u32 scheme_type, scheme_version, i, count;
	const char *kms_uri, *scheme_uri;
	Bool selectiveEncryption=0;
	u32 IVLength=0;
	u32 KeyIndicationLength=0;
	const char *txtHdr=NULL;
	const char *contentID=NULL;
	u32 txtHdrLen=0;
	u64 plainTextLen=0;
	u32 crypt_type=0;
	u32 stsd_idx = ch->owner->stsd ? ch->owner->stsd : 1;

	if (!ch->is_encrypted) return;

	scheme_type = scheme_version = 0;
	kms_uri = scheme_uri = NULL;

	/*ugly fix to detect when an stsd uses both clear and encrypted sample descriptions*/
	count = gf_isom_get_sample_description_count(ch->owner->mov, ch->track);
	if (count>1) {
		u32 first_crypted_stsd = 0;
		u32 nb_same_mtype = 1;
		u32 nb_clear=0, nb_encrypted=0;
		u32 base_subtype = 0;
		Bool first_is_clear = GF_FALSE;
		for (i=0; i<count; i++) {
			u32 mtype = gf_isom_get_media_subtype(ch->owner->mov, ch->track, i+1);
			if ( gf_isom_is_media_encrypted(ch->owner->mov, track, i+1)) {
				gf_isom_get_original_format_type(ch->owner->mov, ch->track, i+1, &mtype);
				nb_encrypted++;
				if (!first_crypted_stsd) first_crypted_stsd = i+1;
			} else {
				nb_clear++;
				if (!i) first_is_clear = GF_TRUE;
			}
			if (!i) base_subtype = mtype;
			else if (base_subtype==mtype) {
				nb_same_mtype++;
			}
		}
		if ((nb_same_mtype==count) && (nb_clear==nb_encrypted)) {
			gf_filter_pid_set_property(ch->pid, GF_PROP_PID_CENC_STSD_MODE, &PROP_UINT(first_is_clear ? 1 : 2) );
			stsd_idx = first_crypted_stsd;
		}

	}

	if (gf_isom_is_ismacryp_media(mov, track, stsd_idx)) {
		gf_isom_get_ismacryp_info(mov, track, stsd_idx, NULL, &scheme_type, &scheme_version, &scheme_uri, &kms_uri, &selectiveEncryption, &IVLength, &KeyIndicationLength);
	} else if (gf_isom_is_omadrm_media(mov, track, stsd_idx)) {
		//u8 hash[20];
		gf_isom_get_omadrm_info(mov, track, stsd_idx, NULL, &scheme_type, &scheme_version, &contentID, &kms_uri, &txtHdr, &txtHdrLen, &plainTextLen, &crypt_type, &selectiveEncryption, &IVLength, &KeyIndicationLength);

		//gf_media_get_file_hash(gf_isom_get_filename(mov), hash);
	} else if (gf_isom_is_cenc_media(mov, track, stsd_idx)) {
		ch->is_cenc = GF_TRUE;

		gf_isom_get_cenc_info(ch->owner->mov, ch->track, stsd_idx, NULL, &scheme_type, &scheme_version);

		//if no PSSH declared, DO update the properties (PSSH is not mandatory)
	} else if (gf_isom_is_adobe_protection_media(mov, track, stsd_idx)) {
		u32 ofmt;
		scheme_version = 1;
		scheme_type = GF_ISOM_ADOBE_SCHEME;
		const char *metadata = NULL;

		gf_isom_get_adobe_protection_info(mov, track, stsd_idx, &ofmt, &scheme_type, &scheme_version, &metadata);
		if (metadata)
			gf_filter_pid_set_property(ch->pid, GF_PROP_PID_ADOBE_CRYPT_META, &PROP_DATA((char *)metadata, (u32) strlen(metadata) ) );
	}

	gf_filter_pid_set_property(ch->pid, GF_PROP_PID_PROTECTION_SCHEME_TYPE, &PROP_4CC(scheme_type) );
	gf_filter_pid_set_property(ch->pid, GF_PROP_PID_PROTECTION_SCHEME_VERSION, &PROP_UINT(scheme_version) );
	if (scheme_uri) gf_filter_pid_set_property(ch->pid, GF_PROP_PID_PROTECTION_SCHEME_URI, &PROP_STRING((char*) scheme_uri) );
	if (kms_uri) gf_filter_pid_set_property(ch->pid, GF_PROP_PID_PROTECTION_KMS_URI, &PROP_STRING((char*) kms_uri) );

	if (selectiveEncryption) gf_filter_pid_set_property(ch->pid, GF_PROP_PID_ISMA_SELECTIVE_ENC, &PROP_BOOL(GF_TRUE) );
	if (IVLength) gf_filter_pid_set_property(ch->pid, GF_PROP_PID_ISMA_IV_LENGTH, &PROP_UINT(IVLength) );
	if (KeyIndicationLength) gf_filter_pid_set_property(ch->pid, GF_PROP_PID_ISMA_KI_LENGTH, &PROP_UINT(KeyIndicationLength) );
	if (crypt_type) gf_filter_pid_set_property(ch->pid, GF_PROP_PID_OMA_CRYPT_TYPE, &PROP_UINT(crypt_type) );
	if (contentID) gf_filter_pid_set_property(ch->pid, GF_PROP_PID_OMA_CID, &PROP_STRING(contentID) );
	if (txtHdr) gf_filter_pid_set_property(ch->pid, GF_PROP_PID_OMA_TXT_HDR, &PROP_STRING(txtHdr) );
	if (plainTextLen) gf_filter_pid_set_property(ch->pid, GF_PROP_PID_OMA_CLEAR_LEN, &PROP_LONGUINT(plainTextLen) );

	if (ch->is_cenc) {
		const u8 *key_info;
		u32 key_info_size;
		u32 container_type;

		isor_declare_pssh(ch);

		gf_isom_cenc_get_default_info(ch->owner->mov, ch->track, stsd_idx, &container_type, &ch->pck_encrypted, &ch->crypt_byte_block, &ch->skip_byte_block, &key_info, &key_info_size);

		gf_filter_pid_set_property(ch->pid, GF_PROP_PID_CENC_STORE, &PROP_4CC(container_type) );

		gf_filter_pid_set_property(ch->pid, GF_PROP_PID_ENCRYPTED, &PROP_BOOL(ch->pck_encrypted) );

		if (ch->skip_byte_block || ch->crypt_byte_block) {
			gf_filter_pid_set_property(ch->pid, GF_PROP_PID_CENC_PATTERN, &PROP_FRAC_INT(ch->skip_byte_block, ch->crypt_byte_block) );
		}
		gf_filter_pid_set_property(ch->pid, GF_PROP_PID_CENC_KEY_INFO, &PROP_DATA((u8 *)key_info, key_info_size) );
		ch->key_info_crc = gf_crc_32(key_info, key_info_size);
	}
}