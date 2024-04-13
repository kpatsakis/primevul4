void gf_isom_push_mdat_end(GF_ISOFile *mov, u64 mdat_end)
{
	u32 i, count;
	if (!mov || !mov->moov) return;

	count = gf_list_count(mov->moov->trackList);
	for (i=0; i<count; i++) {
		u32 j;
		GF_TrafToSampleMap *traf_map;
		GF_TrackBox *trak = gf_list_get(mov->moov->trackList, i);
		if (!trak->Media->information->sampleTable->traf_map) continue;

		traf_map = trak->Media->information->sampleTable->traf_map;
		for (j=traf_map->nb_entries; j>0; j--) {
			if (!traf_map->frag_starts[j-1].mdat_end) {
				traf_map->frag_starts[j-1].mdat_end = mdat_end;
				break;
			}
		}
	}
}