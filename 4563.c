GF_TrackBox *gf_isom_get_track_from_original_id(GF_MovieBox *moov, u32 originalID, u32 originalFile)
{
	u32 i, count;
	if (!moov || !originalID) return NULL;

	count = gf_list_count(moov->trackList);
	for (i = 0; i<count; i++) {
		GF_TrackBox *trak = (GF_TrackBox*)gf_list_get(moov->trackList, i);
		if ((trak->originalFile == originalFile) && (trak->originalID == originalID)) return trak;
	}
	return NULL;
}