GF_Err GetMediaTime(GF_TrackBox *trak, Bool force_non_empty, u64 movieTime, u64 *MediaTime, s64 *SegmentStartTime, s64 *MediaOffset, u8 *useEdit, u64 *next_edit_start_plus_one)
{
#if 0
	GF_Err e;
	u32 sampleNumber, prevSampleNumber;
	u64 firstDTS;
#endif
	u32 i, count;
	Bool last_is_empty = 0;
	u64 time, lastSampleTime;
	s64 mtime;
	GF_EdtsEntry *ent;
	Double scale_ts;
	GF_SampleTableBox *stbl = trak->Media->information->sampleTable;

	if (next_edit_start_plus_one) *next_edit_start_plus_one = 0;
	*useEdit = 1;
	*MediaTime = 0;
	//no segment yet...
	*SegmentStartTime = -1;
	*MediaOffset = -1;
	if (!trak->moov->mvhd->timeScale || !trak->Media->mediaHeader->timeScale || !stbl->SampleSize) {
		return GF_ISOM_INVALID_FILE;
	}

	//no samples...
	if (!stbl->SampleSize->sampleCount) {
		lastSampleTime = 0;
	} else {
		lastSampleTime = trak->Media->mediaHeader->duration;
	}

	//No edits, 1 to 1 mapping
	if (! trak->editBox || !trak->editBox->editList) {
		*MediaTime = movieTime;
		//check this is in our media time line
		if ((*MediaTime > lastSampleTime)
#ifndef GPAC_DISABLE_ISOM_FRAGMENTS
		        && !trak->moov->mov->moof
#endif
		   ) {
			*MediaTime = lastSampleTime;
		}
		*useEdit = 0;
		return GF_OK;
	}
	//browse the edit list and get the time
	scale_ts = trak->Media->mediaHeader->timeScale;
	scale_ts /= trak->moov->mvhd->timeScale;

	time = 0;
	ent = NULL;
	count=gf_list_count(trak->editBox->editList->entryList);
	for (i=0; i<count; i++) {
		ent = (GF_EdtsEntry *)gf_list_get(trak->editBox->editList->entryList, i);
		if ( (time + ent->segmentDuration) * scale_ts > movieTime) {
			if (!force_non_empty || (ent->mediaTime >= 0)) {
				if (next_edit_start_plus_one) *next_edit_start_plus_one = 1 + (u64) ((time + ent->segmentDuration) * scale_ts);
				goto ent_found;
			}
		}
		time += ent->segmentDuration;
		last_is_empty = ent->segmentDuration ? 0 : 1;
	}

	if (last_is_empty) {
		ent = (GF_EdtsEntry *)gf_list_last(trak->editBox->editList->entryList);
		if (ent->mediaRate == 0x10000) {
			*MediaTime = movieTime + ent->mediaTime;
		} else {
			ent = (GF_EdtsEntry *)gf_list_get(trak->editBox->editList->entryList, 0);
			if (ent->mediaRate == -0x10000) {
				u64 dur = (u64) (ent->segmentDuration * scale_ts);
				*MediaTime = (movieTime > dur) ? (movieTime-dur) : 0;
			}
		}
		*useEdit = 0;
		return GF_OK;
	}


	//we had nothing in the list (strange file but compliant...)
	//return the 1 to 1 mapped vale of the last media sample
	if (!ent) {
		*MediaTime = movieTime;
		//check this is in our media time line
		if (*MediaTime > lastSampleTime) *MediaTime = lastSampleTime;
		*useEdit = 0;
		return GF_OK;
	}
	//request for a bigger time that what we can give: return the last sample (undefined behavior...)
	*MediaTime = lastSampleTime;
	return GF_OK;

ent_found:
	//OK, we found our entry, set the SegmentTime
	*SegmentStartTime = time;

	//we request an empty list, there's no media here...
	if (ent->mediaTime < 0) {
		*MediaTime = 0;
		return GF_OK;
	}
	//we request a dwell edit
	if (! ent->mediaRate) {
		*MediaTime = ent->mediaTime;
		//no media offset
		*MediaOffset = 0;
		*useEdit = 2;
		return GF_OK;
	}

	/*WARNING: this can be "-1" when doing searchForward mode (to prevent jumping to next entry)*/
	mtime = ent->mediaTime + movieTime - (time * trak->Media->mediaHeader->timeScale / trak->moov->mvhd->timeScale);
	if (mtime<0) mtime = 0;
	*MediaTime = (u64) mtime;
	*MediaOffset = ent->mediaTime;

#if 0
	//
	//Sanity check: is the requested time valid ? This is to cope with wrong EditLists
	//we have the translated time, but we need to make sure we have a sample at this time ...
	//we have to find a COMPOSITION time
	e = stbl_findEntryForTime(stbl, (u32) *MediaTime, 1, &sampleNumber, &prevSampleNumber);
	if (e) return e;

	//first case: our time is after the last sample DTS (it's a broken editList somehow)
	//set the media time to the last sample
	if (!sampleNumber && !prevSampleNumber) {
		*MediaTime = lastSampleTime;
		return GF_OK;
	}
	//get the appropriated sample
	if (!sampleNumber) sampleNumber = prevSampleNumber;

	stbl_GetSampleDTS(stbl->TimeToSample, sampleNumber, &DTS);
	CTS = 0;
	if (stbl->CompositionOffset) stbl_GetSampleCTS(stbl->CompositionOffset, sampleNumber, &CTS);

	//now get the entry sample (the entry time gives the CTS, and we need the DTS
	e = stbl_findEntryForTime(stbl, (u32) ent->mediaTime, 0, &sampleNumber, &prevSampleNumber);
	if (e) return e;

	//oops, the mediaTime indicates a sample that is not in our media !
	if (!sampleNumber && !prevSampleNumber) {
		*MediaTime = lastSampleTime;
		return GF_ISOM_INVALID_FILE;
	}
	if (!sampleNumber) sampleNumber = prevSampleNumber;

	stbl_GetSampleDTS(stbl->TimeToSample, sampleNumber, &firstDTS);

	//and store the "time offset" of the desired sample in this segment
	//this is weird, used to rebuild the timeStamp when reading from the track, not the
	//media ...
	*MediaOffset = firstDTS;
#endif
	return GF_OK;
}