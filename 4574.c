static void gf_isom_setup_traf_inheritance(GF_ISOFile *mov)
{
	u32 i, count;
	if (!mov->moov->mvex)
		return;
	count = gf_list_count(mov->moov->trackList);

	for (i=0; i<count; i++) {
		u32 refTrackNum=0;
		gf_isom_get_reference(mov, i+1, GF_ISOM_REF_TRIN, 1, &refTrackNum);
		if (refTrackNum) {
			GF_ISOTrackID tkid = gf_isom_get_track_id(mov, i+1);
			GF_ISOTrackID reftkid = gf_isom_get_track_id(mov, refTrackNum);
			GF_TrackExtendsBox *trex = GetTrex(mov->moov, tkid);
			if (trex) trex->inherit_from_traf_id = reftkid;
		}
	}
}