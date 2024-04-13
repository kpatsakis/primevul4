static GF_Err gf_isom_check_sample_desc(GF_TrackBox *trak)
{
	GF_BitStream *bs;
	GF_UnknownBox *a;
	u32 i;
	GF_Err e;
	GF_SampleTableBox *stbl;

	if (!trak->Media || !trak->Media->information) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Track with no media box !\n" ));
		return GF_OK;
	}
	if (!trak->Media->information->sampleTable) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Track with no sample table !\n" ));
		trak->Media->information->sampleTable = (GF_SampleTableBox *) gf_isom_box_new_parent(&trak->Media->information->child_boxes, GF_ISOM_BOX_TYPE_STBL);
	}
	stbl = trak->Media->information->sampleTable;

	if (!stbl->SampleDescription) {
		GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Track with no sample description box !\n" ));
		stbl->SampleDescription = (GF_SampleDescriptionBox *) gf_isom_box_new_parent(&stbl->child_boxes, GF_ISOM_BOX_TYPE_STSD);
		return GF_OK;
	}

	i=0;
	while ((a = (GF_UnknownBox*)gf_list_enum(stbl->SampleDescription->child_boxes, &i))) {
		GF_ProtectionSchemeInfoBox *sinf;
		u32 base_ent_type = 0;
		u32 type = a->type;
		switch (a->type) {
		case GF_ISOM_BOX_TYPE_ENCS:
		case GF_ISOM_BOX_TYPE_ENCA:
		case GF_ISOM_BOX_TYPE_ENCV:
		case GF_ISOM_BOX_TYPE_RESV:
		case GF_ISOM_BOX_TYPE_ENCT:
			sinf = (GF_ProtectionSchemeInfoBox *) gf_isom_box_find_child(a->child_boxes, GF_ISOM_BOX_TYPE_SINF);
			if (!sinf || !sinf->original_format) return GF_ISOM_INVALID_FILE;
			type = sinf->original_format->data_format;
			base_ent_type = ((GF_SampleEntryBox *)a)->internal_type;
			break;
		}

		switch (type) {
		case GF_ISOM_BOX_TYPE_MP4S:
			if (base_ent_type && (base_ent_type != GF_ISOM_SAMPLE_ENTRY_MP4S)) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Protected sample entry %s uses incompatible sample description %s\n", gf_4cc_to_str(a->type), gf_4cc_to_str(type) ));

				return GF_ISOM_INVALID_FILE;
			}
			continue;

		case GF_ISOM_SUBTYPE_3GP_AMR:
		case GF_ISOM_SUBTYPE_3GP_AMR_WB:
		case GF_ISOM_SUBTYPE_3GP_EVRC:
		case GF_ISOM_SUBTYPE_3GP_QCELP:
		case GF_ISOM_SUBTYPE_3GP_SMV:
		case GF_ISOM_BOX_TYPE_MP4A:
		case GF_ISOM_BOX_TYPE_MP3:
		case GF_ISOM_BOX_TYPE_MHA1:
		case GF_ISOM_BOX_TYPE_MHA2:
		case GF_ISOM_BOX_TYPE_MHM1:
		case GF_ISOM_BOX_TYPE_MHM2:
		case GF_ISOM_BOX_TYPE_OPUS:
		case GF_ISOM_BOX_TYPE_AC3:
		case GF_ISOM_BOX_TYPE_EC3:
		case GF_QT_SUBTYPE_RAW_AUD:
		case GF_QT_SUBTYPE_TWOS:
		case GF_QT_SUBTYPE_SOWT:
		case GF_QT_SUBTYPE_FL32:
		case GF_QT_SUBTYPE_FL64:
		case GF_QT_SUBTYPE_IN24:
		case GF_QT_SUBTYPE_IN32:
		case GF_QT_SUBTYPE_ULAW:
		case GF_QT_SUBTYPE_ALAW:
		case GF_QT_SUBTYPE_ADPCM:
		case GF_QT_SUBTYPE_IMA_ADPCM:
		case GF_QT_SUBTYPE_DVCA:
		case GF_QT_SUBTYPE_QDMC:
		case GF_QT_SUBTYPE_QDMC2:
		case GF_QT_SUBTYPE_QCELP:
		case GF_QT_SUBTYPE_kMP3:
		case GF_ISOM_BOX_TYPE_IPCM:
		case GF_ISOM_BOX_TYPE_FPCM:
			if (base_ent_type && (base_ent_type != GF_ISOM_SAMPLE_ENTRY_AUDIO))
				return GF_ISOM_INVALID_FILE;
			continue;

		case GF_ISOM_BOX_TYPE_MP4V:
		case GF_ISOM_SUBTYPE_3GP_H263:
		case GF_ISOM_BOX_TYPE_AVC1:
		case GF_ISOM_BOX_TYPE_AVC2:
		case GF_ISOM_BOX_TYPE_AVC3:
		case GF_ISOM_BOX_TYPE_AVC4:
		case GF_ISOM_BOX_TYPE_SVC1:
		case GF_ISOM_BOX_TYPE_MVC1:
		case GF_ISOM_BOX_TYPE_HVC1:
		case GF_ISOM_BOX_TYPE_HEV1:
		case GF_ISOM_BOX_TYPE_HVC2:
		case GF_ISOM_BOX_TYPE_HEV2:
		case GF_ISOM_BOX_TYPE_HVT1:
		case GF_ISOM_BOX_TYPE_LHV1:
		case GF_ISOM_BOX_TYPE_LHE1:
		case GF_ISOM_BOX_TYPE_AV01:
		case GF_ISOM_BOX_TYPE_VP08:
		case GF_ISOM_BOX_TYPE_VP09:
		case GF_ISOM_BOX_TYPE_AV1C:
		case GF_ISOM_BOX_TYPE_JPEG:
		case GF_ISOM_BOX_TYPE_PNG:
		case GF_ISOM_BOX_TYPE_JP2K:
		case GF_ISOM_BOX_TYPE_MJP2:
		case GF_QT_SUBTYPE_APCH:
		case GF_QT_SUBTYPE_APCO:
		case GF_QT_SUBTYPE_APCN:
		case GF_QT_SUBTYPE_APCS:
		case GF_QT_SUBTYPE_AP4X:
		case GF_QT_SUBTYPE_AP4H:
		case GF_ISOM_BOX_TYPE_VVC1:
		case GF_ISOM_BOX_TYPE_VVI1:
		case GF_QT_SUBTYPE_RAW_VID:
		case GF_QT_SUBTYPE_YUYV:
		case GF_QT_SUBTYPE_UYVY:
		case GF_QT_SUBTYPE_YUV444:
		case GF_QT_SUBTYPE_YUVA444:
		case GF_QT_SUBTYPE_YUV422_10:
		case GF_QT_SUBTYPE_YUV444_10:
		case GF_QT_SUBTYPE_YUV422_16:
		case GF_QT_SUBTYPE_YUV420:
		case GF_QT_SUBTYPE_I420:
		case GF_QT_SUBTYPE_IYUV:
		case GF_QT_SUBTYPE_YV12:
		case GF_QT_SUBTYPE_YVYU:
		case GF_QT_SUBTYPE_RGBA:
		case GF_QT_SUBTYPE_ABGR:
		case GF_ISOM_BOX_TYPE_DVHE:
		case GF_ISOM_BOX_TYPE_DVH1:
		case GF_ISOM_BOX_TYPE_DVA1:
		case GF_ISOM_BOX_TYPE_DVAV:
		case GF_ISOM_BOX_TYPE_DAV1:
			if (base_ent_type && (base_ent_type != GF_ISOM_SAMPLE_ENTRY_VIDEO))
				return GF_ISOM_INVALID_FILE;
			continue;


		case GF_ISOM_BOX_TYPE_METX:
		case GF_ISOM_BOX_TYPE_METT:
		case GF_ISOM_BOX_TYPE_STXT:
		case GF_ISOM_BOX_TYPE_TX3G:
		case GF_ISOM_BOX_TYPE_TEXT:
		case GF_ISOM_BOX_TYPE_GHNT:
		case GF_ISOM_BOX_TYPE_RTP_STSD:
		case GF_ISOM_BOX_TYPE_SRTP_STSD:
		case GF_ISOM_BOX_TYPE_FDP_STSD:
		case GF_ISOM_BOX_TYPE_RRTP_STSD:
		case GF_ISOM_BOX_TYPE_RTCP_STSD:
		case GF_ISOM_BOX_TYPE_DIMS:
		case GF_ISOM_BOX_TYPE_LSR1:
		case GF_ISOM_BOX_TYPE_WVTT:
		case GF_ISOM_BOX_TYPE_STPP:
		case GF_ISOM_BOX_TYPE_SBTT:
			if (base_ent_type && (base_ent_type != GF_ISOM_SAMPLE_ENTRY_GENERIC))
				return GF_ISOM_INVALID_FILE;
			continue;


		case GF_ISOM_BOX_TYPE_UNKNOWN:
			break;
		default:
			if (gf_box_valid_in_parent((GF_Box *) a, "stsd")) {
				continue;
			}
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Unexpected box %s in stsd!\n", gf_4cc_to_str(a->type)));
			continue;
		}
		//we are sure to have an unknown box here
		assert(a->type==GF_ISOM_BOX_TYPE_UNKNOWN);

		if (!a->data || (a->dataSize<8) ) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Sample description %s does not have at least 8 bytes!\n", gf_4cc_to_str(a->original_4cc) ));
			continue;
		}
		else if (a->dataSize > a->size) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Sample description %s has wrong data size %d!\n", gf_4cc_to_str(a->original_4cc), a->dataSize));
			continue;
		}

		/*only process visual or audio
		note: no need for new_box_parent here since we always store sample descriptions in child_boxes*/
		switch (trak->Media->handler->handlerType) {
        case GF_ISOM_MEDIA_VISUAL:
		case GF_ISOM_MEDIA_AUXV:
		case GF_ISOM_MEDIA_PICT:
		{
			GF_GenericVisualSampleEntryBox *genv = (GF_GenericVisualSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRV);
			bs = gf_bs_new(a->data, a->dataSize, GF_BITSTREAM_READ);
			genv->size = a->size-8;
			gf_isom_video_sample_entry_read((GF_VisualSampleEntryBox *) genv, bs);

			stsd_switch_box(bs, (GF_Box *) genv, a, &genv->data, &genv->data_size, &genv->EntryType, trak->Media->information->sampleTable->SampleDescription, i-1);
		}
		break;
		case GF_ISOM_MEDIA_AUDIO:
		{
			GF_GenericAudioSampleEntryBox *gena = (GF_GenericAudioSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRA);
			gena->size = a->size-8;
			bs = gf_bs_new(a->data, a->dataSize, GF_BITSTREAM_READ);
			gf_isom_audio_sample_entry_read((GF_AudioSampleEntryBox *) gena, bs);

			stsd_switch_box(bs, (GF_Box *) gena, a, &gena->data, &gena->data_size, &gena->EntryType, trak->Media->information->sampleTable->SampleDescription, i-1);
		}
		break;

		default:
		{
			GF_GenericSampleEntryBox *genm = (GF_GenericSampleEntryBox *) gf_isom_box_new(GF_ISOM_BOX_TYPE_GNRM);
			genm->size = a->size-8;
			bs = gf_bs_new(a->data, a->dataSize, GF_BITSTREAM_READ);

			e = gf_isom_base_sample_entry_read((GF_SampleEntryBox *)genm, bs);
			if (e) return e;

			stsd_switch_box(bs, (GF_Box *) genm, a, &genm->data, &genm->data_size, &genm->EntryType, trak->Media->information->sampleTable->SampleDescription, i-1);
		}
		break;
		}
	}
	return GF_OK;
}