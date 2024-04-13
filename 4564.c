u32 gf_isom_sample_get_subsample_entry(GF_ISOFile *movie, u32 track, u32 sampleNumber, u32 flags, GF_SubSampleInfoEntry **sub_sample)
{
	u32 i, count, last_sample;
	GF_SubSampleInformationBox *sub_samples=NULL;
	GF_TrackBox *trak = gf_isom_get_track_from_file(movie, track);
	if (sub_sample) *sub_sample = NULL;
	if (!track) return 0;
	if (!trak->Media || !trak->Media->information->sampleTable || !trak->Media->information->sampleTable->sub_samples) return 0;
	count = gf_list_count(trak->Media->information->sampleTable->sub_samples);
	for (i=0; i<count; i++) {
		sub_samples = gf_list_get(trak->Media->information->sampleTable->sub_samples, i);
		if (sub_samples->flags==flags) break;
		sub_samples = NULL;
	}
	if (!sub_samples) return 0;

	last_sample = 0;
	count = gf_list_count(sub_samples->Samples);
	for (i=0; i<count; i++) {
		GF_SubSampleInfoEntry *pSamp = (GF_SubSampleInfoEntry *) gf_list_get(sub_samples->Samples, i);
		if (last_sample + pSamp->sample_delta == sampleNumber) {
			if (sub_sample) *sub_sample = pSamp;
			return gf_list_count(pSamp->SubSamples);
		}
		last_sample += pSamp->sample_delta;
	}
	return 0;
}