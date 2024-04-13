static void FixSDTPInTRAF(GF_MovieFragmentBox *moof)
{
	u32 k;
	if (!moof)
		return;

	for (k = 0; k < gf_list_count(moof->TrackList); k++) {
		GF_TrackFragmentBox *traf = gf_list_get(moof->TrackList, k);
		if (traf->sdtp) {
			GF_TrackFragmentRunBox *trun;
			u32 j = 0, sample_index = 0;

			if (traf->sdtp->sampleCount == gf_list_count(traf->TrackRuns)) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] Warning: TRAF box of track id=%u contains a SDTP. Converting to TRUN sample flags.\n", traf->tfhd->trackID));
			}

			while ((trun = (GF_TrackFragmentRunBox*)gf_list_enum(traf->TrackRuns, &j))) {
				u32 i;
				trun->flags |= GF_ISOM_TRUN_FLAGS;
				for (i=0; i<trun->nb_samples; i++) {
					GF_TrunEntry *entry = &trun->samples[i];
					const u8 info = traf->sdtp->sample_info[sample_index];
					entry->flags |= GF_ISOM_GET_FRAG_DEPEND_FLAGS(info >> 6, info >> 4, info >> 2, info);
					sample_index++;
					if (sample_index > traf->sdtp->sampleCount) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Error: TRAF box of track id=%u contained an inconsistent SDTP.\n", traf->tfhd->trackID));
						return;
					}
				}
			}
			if (sample_index < traf->sdtp->sampleCount) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Error: TRAF box of track id=%u list less samples than SDTP.\n", traf->tfhd->trackID));
			}
			gf_isom_box_del_parent(&traf->child_boxes, (GF_Box*)traf->sdtp);
			traf->sdtp = NULL;
		}
	}
}