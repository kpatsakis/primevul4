static int mov_read_tfhd(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVFragment *frag = &c->fragment;
    MOVTrackExt *trex = NULL;
    MOVFragmentIndex* index = NULL;
    int flags, track_id, i, found = 0;

    avio_r8(pb); /* version */
    flags = avio_rb24(pb);

    track_id = avio_rb32(pb);
    if (!track_id)
        return AVERROR_INVALIDDATA;
    frag->track_id = track_id;
    for (i = 0; i < c->trex_count; i++)
        if (c->trex_data[i].track_id == frag->track_id) {
            trex = &c->trex_data[i];
            break;
        }
    if (!trex) {
        av_log(c->fc, AV_LOG_ERROR, "could not find corresponding trex\n");
        return AVERROR_INVALIDDATA;
    }

    frag->base_data_offset = flags & MOV_TFHD_BASE_DATA_OFFSET ?
                             avio_rb64(pb) : flags & MOV_TFHD_DEFAULT_BASE_IS_MOOF ?
                             frag->moof_offset : frag->implicit_offset;
    frag->stsd_id  = flags & MOV_TFHD_STSD_ID ? avio_rb32(pb) : trex->stsd_id;

    frag->duration = flags & MOV_TFHD_DEFAULT_DURATION ?
                     avio_rb32(pb) : trex->duration;
    frag->size     = flags & MOV_TFHD_DEFAULT_SIZE ?
                     avio_rb32(pb) : trex->size;
    frag->flags    = flags & MOV_TFHD_DEFAULT_FLAGS ?
                     avio_rb32(pb) : trex->flags;
    frag->time     = AV_NOPTS_VALUE;
    for (i = 0; i < c->fragment_index_count; i++) {
        int j;
        MOVFragmentIndex* candidate = c->fragment_index_data[i];
        if (candidate->track_id == frag->track_id) {
            av_log(c->fc, AV_LOG_DEBUG,
                   "found fragment index for track %u\n", frag->track_id);
            index = candidate;
            for (j = index->current_item; j < index->item_count; j++) {
                if (frag->implicit_offset == index->items[j].moof_offset) {
                    av_log(c->fc, AV_LOG_DEBUG, "found fragment index entry "
                            "for track %u and moof_offset %"PRId64"\n",
                            frag->track_id, index->items[j].moof_offset);
                    frag->time = index->items[j].time;
                    index->current_item = j + 1;
                    found = 1;
                    break;
                }
            }
            if (found)
                break;
        }
    }
    if (index && !found) {
        av_log(c->fc, AV_LOG_DEBUG, "track %u has a fragment index but "
               "it doesn't have an (in-order) entry for moof_offset "
               "%"PRId64"\n", frag->track_id, frag->implicit_offset);
    }
    av_log(c->fc, AV_LOG_TRACE, "frag flags 0x%x\n", frag->flags);
    return 0;
}
