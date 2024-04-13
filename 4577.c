GF_Err MergeFragment(GF_MovieFragmentBox *moof, GF_ISOFile *mov)
{
	GF_Err e;
	u32 i, j;
	u64 MaxDur;
	GF_TrackFragmentBox *traf;
	GF_TrackBox *trak;
	u64 base_data_offset;

	MaxDur = 0;

	//we shall have a MOOV and its MVEX BEFORE any MOOF
	if (!mov->moov || !mov->moov->mvex) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Error: %s not received before merging fragment\n", mov->moov ? "mvex" : "moov" ));
		return GF_ISOM_INVALID_FILE;
	}
	//and all fragments should be continous but:
	//- dash with dependent representations may likely give R1(moofSN 1, 3, 5, 7) plus R2(moofSN 2, 4, 6, 8)
	//- smooth muxed in a single file may end up with V(1),A(1), V(2),A(2) ...
	//we do not throw an error if not as we may still want to be able to concatenate dependent representations in DASH and
	if (mov->NextMoofNumber && moof->mfhd && (mov->NextMoofNumber >= moof->mfhd->sequence_number)) {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[iso file] wrong sequence number: got %d but last one was %d\n", moof->mfhd->sequence_number, mov->NextMoofNumber));
	}

	base_data_offset = mov->current_top_box_start;
	if (moof->compressed_diff)
		base_data_offset -= moof->compressed_diff;

	i=0;
	while ((traf = (GF_TrackFragmentBox*)gf_list_enum(moof->TrackList, &i))) {
		u32 prev_sample_count;
		if (!traf->tfhd) {
			trak = NULL;
			traf->trex = NULL;
		} else if (mov->is_smooth) {
			trak = gf_list_get(mov->moov->trackList, 0);
			traf->trex = (GF_TrackExtendsBox*)gf_list_get(mov->moov->mvex->TrackExList, 0);
			assert(traf->trex);
			traf->trex->trackID = trak->Header->trackID = traf->tfhd->trackID;
		} else {
			trak = gf_isom_get_track_from_id(mov->moov, traf->tfhd->trackID);
			j=0;
			while ((traf->trex = (GF_TrackExtendsBox*)gf_list_enum(mov->moov->mvex->TrackExList, &j))) {
				if (traf->trex->trackID == traf->tfhd->trackID) break;
				traf->trex = NULL;
			}
		}

		if (!trak || !traf->trex) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Error: Cannot find fragment track with ID %d\n", traf->tfhd ? traf->tfhd->trackID : 0));
			return GF_ISOM_INVALID_FILE;
		}

		prev_sample_count = trak->Media->information->sampleTable->SampleSize ? trak->Media->information->sampleTable->SampleSize->sampleCount : 0;
		e = MergeTrack(trak, traf, moof, mov->current_top_box_start, moof->compressed_diff, &base_data_offset);
		if (e) return e;
		trak->first_traf_merged = GF_TRUE;

		trak->present_in_scalable_segment = 1;

		//update trak duration
		SetTrackDuration(trak);
		if (trak->Header->duration > MaxDur)
			MaxDur = trak->Header->duration;

		//we have PSSH per moov, internally remap as a sample group of type PSSH
		if (gf_list_count(moof->PSSHs)) {
			u8 *pssh_data;
			u32 pssh_len;
			u32 j, k, nb_pssh = gf_list_count(moof->PSSHs);
			GF_BitStream *pssh_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
			gf_bs_write_u32(pssh_bs, nb_pssh);
			for (j=0; j<nb_pssh; j++) {
				GF_ProtectionSystemHeaderBox *pssh = gf_list_get(moof->PSSHs, j);
				gf_bs_write_data(pssh_bs, pssh->SystemID, 16);
				gf_bs_write_u32(pssh_bs, pssh->version);
				if (pssh->version) {
					gf_bs_write_u32(pssh_bs, pssh->KID_count);
					for (k=0; k<pssh->KID_count; k++) {
						gf_bs_write_data(pssh_bs, pssh->KIDs[k], 16);
					}
				}
				gf_bs_write_u32(pssh_bs, pssh->private_data_size);
				gf_bs_write_data(pssh_bs, pssh->private_data, pssh->private_data_size);
			}
			gf_bs_get_content(pssh_bs, &pssh_data, &pssh_len);
			gf_bs_del(pssh_bs);

			gf_isom_set_sample_group_description_internal(mov, gf_list_find(mov->moov->trackList, trak)+1, 1+prev_sample_count, GF_4CC('P','S','S','H'), 0, pssh_data, pssh_len, GF_FALSE);
			gf_free(pssh_data);
		}


		//we have emsg, internally remap as a sample group of type EMSG
		if (gf_list_count(mov->emsgs)) {
			u8 *emsg_data;
			u32 emsg_len;
			u32 j, nb_emsg = gf_list_count(mov->emsgs);
			GF_BitStream *emsg_bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
			for (j=0; j<nb_emsg; j++) {
				GF_Box *emsg = gf_list_get(mov->emsgs, j);
				gf_isom_box_write(emsg, emsg_bs);
			}
			gf_bs_get_content(emsg_bs, &emsg_data, &emsg_len);
			gf_bs_del(emsg_bs);

			gf_isom_set_sample_group_description_internal(mov, gf_list_find(mov->moov->trackList, trak)+1, 1+prev_sample_count, GF_4CC('E','M','S','G'), 0, emsg_data, emsg_len, GF_FALSE);
			gf_free(emsg_data);
		}
	}
	if (mov->emsgs) {
		gf_isom_box_array_del(mov->emsgs);
		mov->emsgs = NULL;
	}

	if (moof->child_boxes) {
		GF_Box *a;
		i = 0;
		while ((a = (GF_Box *)gf_list_enum(moof->child_boxes, &i))) {
			if (a->type == GF_ISOM_BOX_TYPE_PSSH) {
				GF_ProtectionSystemHeaderBox *pssh = (GF_ProtectionSystemHeaderBox *)gf_isom_box_new_parent(&mov->moov->child_boxes, GF_ISOM_BOX_TYPE_PSSH);
				if (!pssh) return GF_OUT_OF_MEM;
				memmove(pssh->SystemID, ((GF_ProtectionSystemHeaderBox *)a)->SystemID, 16);
				if (((GF_ProtectionSystemHeaderBox *)a)->KIDs && ((GF_ProtectionSystemHeaderBox *)a)->KID_count > 0) {
					pssh->KID_count = ((GF_ProtectionSystemHeaderBox *)a)->KID_count;
					pssh->KIDs = (bin128 *)gf_malloc(pssh->KID_count*sizeof(bin128));
					if (!pssh->KIDs) return GF_OUT_OF_MEM;

					memmove(pssh->KIDs, ((GF_ProtectionSystemHeaderBox *)a)->KIDs, pssh->KID_count*sizeof(bin128));
				}
				pssh->private_data_size = ((GF_ProtectionSystemHeaderBox *)a)->private_data_size;
				pssh->private_data = (u8 *)gf_malloc(pssh->private_data_size*sizeof(char));
				if (!pssh->private_data) return GF_OUT_OF_MEM;
				memmove(pssh->private_data, ((GF_ProtectionSystemHeaderBox *)a)->private_data, pssh->private_data_size);
				pssh->moof_defined = 1;
				mov->has_pssh_moof = GF_TRUE;
			}
		}
	}

	mov->NextMoofNumber = moof->mfhd ? moof->mfhd->sequence_number : 0;
	//update movie duration
	if (mov->moov->mvhd->duration < MaxDur) mov->moov->mvhd->duration = MaxDur;
	return GF_OK;
}