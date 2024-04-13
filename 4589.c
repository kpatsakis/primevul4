Bool gf_isom_get_subsample_types(GF_ISOFile *movie, u32 track, u32 subs_index, u32 *flags)
{
	GF_SubSampleInformationBox *sub_samples=NULL;
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, track);

	if (!track || !subs_index) return GF_FALSE;
	if (!trak->Media || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->sub_samples) return GF_FALSE;
	sub_samples = gf_list_get(trak->Media->information->sampleTable->sub_samples, subs_index-1);
	if (!sub_samples) return GF_FALSE;
	*flags = sub_samples->flags;
	return GF_TRUE;
}