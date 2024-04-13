u32 gf_isom_sample_get_subsamples_count(GF_ISOFile *movie, u32 track)
{
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, track);
	if (!track) return 0;
	if (!trak->Media || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->sub_samples) return 0;
	return gf_list_count(trak->Media->information->sampleTable->sub_samples);
}