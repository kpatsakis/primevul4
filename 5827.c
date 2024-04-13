static void isoffin_purge_mem(ISOMReader *read, u64 min_offset)
{
	u32 i, count;
	u64 top_offset;
	u32 nb_bytes_to_purge;
	u64 bytes_missing;

	//purge every
	if (read->mstore_purge && (min_offset - read->last_min_offset < read->mstore_purge))
		return;

	if (read->frag_type) {
		//get position of current box being parsed - if new offset is greater than this box we cannot remove
		//bytes (we would trash the top-level box header)
		gf_isom_get_current_top_box_offset(read->mov, &top_offset);
		if (top_offset<min_offset) {
			return;
		}
	}
	read->last_min_offset = min_offset;

	assert(min_offset>=read->bytes_removed);
	//min_offset is given in absolute file position
	nb_bytes_to_purge = (u32) (min_offset - read->bytes_removed);
	assert(nb_bytes_to_purge<=read->mem_blob.size);

	memmove(read->mem_blob.data, read->mem_blob.data+nb_bytes_to_purge, read->mem_blob.size - nb_bytes_to_purge);
	read->mem_blob.size -= nb_bytes_to_purge;
	read->bytes_removed += nb_bytes_to_purge;
	gf_isom_set_removed_bytes(read->mov, read->bytes_removed);

	GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[IsoMedia] mem mode %d bytes in mem, "LLU" bytes trashed since start\n", read->mem_blob.size, read->bytes_removed));

	//force a refresh
	gf_isom_refresh_fragmented(read->mov, &bytes_missing, read->mem_url);

	if (!read->frag_type)
		return;

	//fragmented file, cleanup sample tables
	count = gf_list_count(read->channels);
	for (i=0; i<count; i++) {
		ISOMChannel *ch = gf_list_get(read->channels, i);
		u32 num_samples;
		u32 prev_samples = gf_isom_get_sample_count(read->mov, ch->track);
		//don't run this too often
		if (ch->sample_num<=1+read->mstore_samples) continue;

		num_samples = ch->sample_num-1;
		if (num_samples>=prev_samples) continue;

		if (gf_isom_purge_samples(read->mov, ch->track, num_samples) == GF_OK)
			ch->sample_num = 1;

		num_samples = gf_isom_get_sample_count(read->mov, ch->track);
		assert(ch->sample_num<=num_samples);
		GF_LOG(GF_LOG_DEBUG, GF_LOG_CONTAINER, ("[IsoMedia] mem mode %d samples now in track %d (prev %d)\n", num_samples, ch->track_id, prev_samples));
	}
}