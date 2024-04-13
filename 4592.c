GF_TrackBox *gf_isom_get_track_from_file(GF_ISOFile *movie, u32 trackNumber)
{
	GF_TrackBox *trak;
	if (!movie) return NULL;
	trak = gf_isom_get_track(movie->moov, trackNumber);
	if (!trak) movie->LastError = GF_BAD_PARAM;
	return trak;
}