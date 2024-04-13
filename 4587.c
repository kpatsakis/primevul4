GF_Err GetNextMediaTime(GF_TrackBox *trak, u64 movieTime, u64 *OutMovieTime)
{
	u32 i;
	u64 time;
	GF_EdtsEntry *ent;

	*OutMovieTime = 0;
	if (! trak->editBox || !trak->editBox->editList) return GF_BAD_PARAM;

	time = 0;
	ent = NULL;
	i=0;
	while ((ent = (GF_EdtsEntry *)gf_list_enum(trak->editBox->editList->entryList, &i))) {
		if (gf_timestamp_greater_or_equal(time, trak->moov->mvhd->timeScale, movieTime, trak->Media->mediaHeader->timeScale)) {
			/*skip empty edits*/
			if (ent->mediaTime >= 0) {
				*OutMovieTime = time * trak->Media->mediaHeader->timeScale / trak->moov->mvhd->timeScale;
				if (*OutMovieTime>0) *OutMovieTime -= 1;
				return GF_OK;
			}
		}
		time += ent->segmentDuration;
	}
	//request for a bigger time that what we can give: return the last sample (undefined behavior...)
	*OutMovieTime = trak->moov->mvhd->duration;
	return GF_EOS;
}