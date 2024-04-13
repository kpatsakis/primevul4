static int mov_read_trun(MOVContext *c, AVIOContext *pb, MOVAtom atom)
{
    MOVFragment *frag = &c->fragment;
    AVStream *st = NULL;
    MOVStreamContext *sc;
    MOVStts *ctts_data;
    uint64_t offset;
    int64_t dts;
    int data_offset = 0;
    unsigned entries, first_sample_flags = frag->flags;
    int flags, distance, i;

    for (i = 0; i < c->fc->nb_streams; i++) {
        if (c->fc->streams[i]->id == frag->track_id) {
            st = c->fc->streams[i];
            break;
        }
    }
    if (!st) {
        av_log(c->fc, AV_LOG_ERROR, "could not find corresponding track id %u\n", frag->track_id);
        return AVERROR_INVALIDDATA;
    }
    sc = st->priv_data;
    if (sc->pseudo_stream_id+1 != frag->stsd_id && sc->pseudo_stream_id != -1)
        return 0;
    avio_r8(pb); /* version */
    flags = avio_rb24(pb);
    entries = avio_rb32(pb);
    av_log(c->fc, AV_LOG_TRACE, "flags 0x%x entries %u\n", flags, entries);

    if ((uint64_t)entries+sc->ctts_count >= UINT_MAX/sizeof(*sc->ctts_data))
        return AVERROR_INVALIDDATA;
    if (flags & MOV_TRUN_DATA_OFFSET)        data_offset        = avio_rb32(pb);
    if (flags & MOV_TRUN_FIRST_SAMPLE_FLAGS) first_sample_flags = avio_rb32(pb);
    dts    = sc->track_end - sc->time_offset;
    offset = frag->base_data_offset + data_offset;
    distance = 0;
    av_log(c->fc, AV_LOG_TRACE, "first sample flags 0x%x\n", first_sample_flags);
    for (i = 0; i < entries && !pb->eof_reached; i++) {
        unsigned sample_size = frag->size;
        int sample_flags = i ? frag->flags : first_sample_flags;
        unsigned sample_duration = frag->duration;
        unsigned ctts_duration = 0;
        int keyframe = 0;
        int ctts_index = 0;
        int old_nb_index_entries = st->nb_index_entries;

        if (flags & MOV_TRUN_SAMPLE_DURATION) sample_duration = avio_rb32(pb);
        if (flags & MOV_TRUN_SAMPLE_SIZE)     sample_size     = avio_rb32(pb);
        if (flags & MOV_TRUN_SAMPLE_FLAGS)    sample_flags    = avio_rb32(pb);
        if (flags & MOV_TRUN_SAMPLE_CTS)      ctts_duration   = avio_rb32(pb);

        mov_update_dts_shift(sc, ctts_duration);
        if (frag->time != AV_NOPTS_VALUE) {
            if (c->use_mfra_for == FF_MOV_FLAG_MFRA_PTS) {
                int64_t pts = frag->time;
                av_log(c->fc, AV_LOG_DEBUG, "found frag time %"PRId64
                        " sc->dts_shift %d ctts.duration %d"
                        " sc->time_offset %"PRId64" flags & MOV_TRUN_SAMPLE_CTS %d\n", pts,
                        sc->dts_shift, ctts_duration,
                        sc->time_offset, flags & MOV_TRUN_SAMPLE_CTS);
                dts = pts - sc->dts_shift;
                if (flags & MOV_TRUN_SAMPLE_CTS) {
                    dts -= ctts_duration;
                } else {
                    dts -= sc->time_offset;
                }
                av_log(c->fc, AV_LOG_DEBUG, "calculated into dts %"PRId64"\n", dts);
            } else {
                dts = frag->time - sc->time_offset;
                av_log(c->fc, AV_LOG_DEBUG, "found frag time %"PRId64
                        ", using it for dts\n", dts);
            }
            frag->time = AV_NOPTS_VALUE;
        }

        if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            keyframe = 1;
        else
            keyframe =
                !(sample_flags & (MOV_FRAG_SAMPLE_FLAG_IS_NON_SYNC |
                                  MOV_FRAG_SAMPLE_FLAG_DEPENDS_YES));
        if (keyframe)
            distance = 0;
        ctts_index = av_add_index_entry(st, offset, dts, sample_size, distance,
                                        keyframe ? AVINDEX_KEYFRAME : 0);
        if (ctts_index >= 0 && old_nb_index_entries < st->nb_index_entries) {
            unsigned int size_needed = st->nb_index_entries * sizeof(*sc->ctts_data);
            unsigned int request_size = size_needed > sc->ctts_allocated_size ?
                FFMAX(size_needed, 2 * sc->ctts_allocated_size) : size_needed;
            unsigned int old_ctts_size = sc->ctts_allocated_size;
            ctts_data = av_fast_realloc(sc->ctts_data, &sc->ctts_allocated_size, request_size);
            if (!ctts_data) {
                av_freep(&sc->ctts_data);
                return AVERROR(ENOMEM);
            }
            sc->ctts_data = ctts_data;

            memset((uint8_t*)(sc->ctts_data) + old_ctts_size, 0, sc->ctts_allocated_size - old_ctts_size);

            if (ctts_index != old_nb_index_entries) {
                memmove(sc->ctts_data + ctts_index + 1, sc->ctts_data + ctts_index,
                        sizeof(*sc->ctts_data) * (sc->ctts_count - ctts_index));
                if (ctts_index <= sc->current_sample) {
                    sc->current_sample++;
                }
            }

            sc->ctts_data[ctts_index].count = 1;
            sc->ctts_data[ctts_index].duration = ctts_duration;
            sc->ctts_count++;
        } else {
            av_log(c->fc, AV_LOG_ERROR, "Failed to add index entry\n");
        }

        av_log(c->fc, AV_LOG_TRACE, "AVIndex stream %d, sample %d, offset %"PRIx64", dts %"PRId64", "
                "size %u, distance %d, keyframe %d\n", st->index, ctts_index,
                offset, dts, sample_size, distance, keyframe);
        distance++;
        dts += sample_duration;
        offset += sample_size;
        sc->data_size += sample_size;
        sc->duration_for_fps += sample_duration;
        sc->nb_frames_for_fps ++;
    }

    if (pb->eof_reached)
        return AVERROR_EOF;

    frag->implicit_offset = offset;

    sc->track_end = dts + sc->time_offset;
    if (st->duration < sc->track_end)
        st->duration = sc->track_end;

    return 0;
}
