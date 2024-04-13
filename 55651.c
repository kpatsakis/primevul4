static int mxf_read_seek(AVFormatContext *s, int stream_index, int64_t sample_time, int flags)
{
    AVStream *st = s->streams[stream_index];
    int64_t seconds;
    MXFContext* mxf = s->priv_data;
    int64_t seekpos;
    int i, ret;
    MXFIndexTable *t;
    MXFTrack *source_track = st->priv_data;

    if(st->codecpar->codec_type == AVMEDIA_TYPE_DATA)
        return 0;

    /* if audio then truncate sample_time to EditRate */
    if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
        sample_time = av_rescale_q(sample_time, st->time_base,
                                   av_inv_q(source_track->edit_rate));

    if (mxf->nb_index_tables <= 0) {
    if (!s->bit_rate)
        return AVERROR_INVALIDDATA;
    if (sample_time < 0)
        sample_time = 0;
    seconds = av_rescale(sample_time, st->time_base.num, st->time_base.den);

    seekpos = avio_seek(s->pb, (s->bit_rate * seconds) >> 3, SEEK_SET);
    if (seekpos < 0)
        return seekpos;

    ff_update_cur_dts(s, st, sample_time);
    mxf->current_edit_unit = sample_time;
    } else {
        t = &mxf->index_tables[0];

        /* clamp above zero, else ff_index_search_timestamp() returns negative
         * this also means we allow seeking before the start */
        sample_time = FFMAX(sample_time, 0);

        if (t->fake_index) {
            /* The first frames may not be keyframes in presentation order, so
             * we have to advance the target to be able to find the first
             * keyframe backwards... */
            if (!(flags & AVSEEK_FLAG_ANY) &&
                (flags & AVSEEK_FLAG_BACKWARD) &&
                t->ptses[0] != AV_NOPTS_VALUE &&
                sample_time < t->ptses[0] &&
                (t->fake_index[t->ptses[0]].flags & AVINDEX_KEYFRAME))
                sample_time = t->ptses[0];

            /* behave as if we have a proper index */
            if ((sample_time = ff_index_search_timestamp(t->fake_index, t->nb_ptses, sample_time, flags)) < 0)
                return sample_time;
            /* get the stored order index from the display order index */
            sample_time += t->offsets[sample_time];
        } else {
            /* no IndexEntryArray (one or more CBR segments)
             * make sure we don't seek past the end */
            sample_time = FFMIN(sample_time, source_track->original_duration - 1);
        }

        if ((ret = mxf_edit_unit_absolute_offset(mxf, t, sample_time, &sample_time, &seekpos, 1)) < 0)
            return ret;

        ff_update_cur_dts(s, st, sample_time);
        mxf->current_edit_unit = sample_time;
        avio_seek(s->pb, seekpos, SEEK_SET);
    }

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *cur_st = s->streams[i];
        MXFTrack *cur_track = cur_st->priv_data;
        uint64_t current_sample_count = 0;
        if (cur_st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            ret = mxf_compute_sample_count(mxf, i, &current_sample_count);
            if (ret < 0)
                return ret;

            cur_track->sample_count = current_sample_count;
        }
    }
    return 0;
}
