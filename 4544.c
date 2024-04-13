GF_TrackBox *gf_isom_get_track_from_id(GF_MovieBox *moov, GF_ISOTrackID trackID)
{
	u32 i, count;
	if (!moov || !trackID) return NULL;

	count = gf_list_count(moov->trackList);
	for (i = 0; i<count; i++) {
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(moov->trackList, i);
		if (trak->Header->trackID == trackID) return trak;
	}
	return NULL;
}