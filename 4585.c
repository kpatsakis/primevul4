GF_Err gf_isom_add_subsample_info(GF_SubSampleInformationBox *sub_samples, u32 sampleNumber, u32 subSampleSize, u8 priority, u32 reserved, Bool discardable)
{
	u32 i, count, last_sample;
	GF_SubSampleInfoEntry *pSamp;
	GF_SubSampleEntry *pSubSamp;

	pSamp = NULL;
	last_sample = 0;
	count = gf_list_count(sub_samples->Samples);
	for (i=0; i<count; i++) {
		pSamp = (GF_SubSampleInfoEntry*) gf_list_get(sub_samples->Samples, i);
		/*TODO - do we need to support insertion of subsample info ?*/
		if (last_sample + pSamp->sample_delta > sampleNumber) return GF_NOT_SUPPORTED;
		if (last_sample + pSamp->sample_delta == sampleNumber) break;
		last_sample += pSamp->sample_delta;
		pSamp = NULL;
	}

	if (!pSamp) {
		GF_SAFEALLOC(pSamp, GF_SubSampleInfoEntry);
		if (!pSamp) return GF_OUT_OF_MEM;
		pSamp->SubSamples = gf_list_new();
		if (!pSamp->SubSamples ) {
			gf_free(pSamp);
			return GF_OUT_OF_MEM;
		}
		pSamp->sample_delta = sampleNumber - last_sample;
		gf_list_add(sub_samples->Samples, pSamp);
	}

	if ((subSampleSize>0xFFFF) && !sub_samples->version) {
		sub_samples->version = 1;
	}
	/*remove last subsample info*/
	if (!subSampleSize) {
		pSubSamp = gf_list_last(pSamp->SubSamples);
		gf_list_rem_last(pSamp->SubSamples);
		gf_free(pSubSamp);
		if (!gf_list_count(pSamp->SubSamples)) {
			gf_list_del_item(sub_samples->Samples, pSamp);
			gf_list_del(pSamp->SubSamples);
			gf_free(pSamp);
		}
		return GF_OK;
	}
	/*add subsample*/
	GF_SAFEALLOC(pSubSamp, GF_SubSampleEntry);
	if (!pSubSamp) return GF_OUT_OF_MEM;
	pSubSamp->subsample_size = subSampleSize;
	pSubSamp->subsample_priority = priority;
	pSubSamp->reserved = reserved;
	pSubSamp->discardable = discardable;
	return gf_list_add(pSamp->SubSamples, pSubSamp);
}