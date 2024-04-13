static int ni_prepare_read(AVFormatContext *s)
{
    AVIContext *avi = s->priv_data;
    int best_stream_index = 0;
    AVStream *best_st     = NULL;
    AVIStream *best_ast;
    int64_t best_ts = INT64_MAX;
    int i;

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st   = s->streams[i];
        AVIStream *ast = st->priv_data;
        int64_t ts     = ast->frame_offset;
        int64_t last_ts;

        if (!st->nb_index_entries)
            continue;

        last_ts = st->index_entries[st->nb_index_entries - 1].timestamp;
        if (!ast->remaining && ts > last_ts)
            continue;

        ts = av_rescale_q(ts, st->time_base,
                          (AVRational) { FFMAX(1, ast->sample_size),
                                         AV_TIME_BASE });

        av_log(s, AV_LOG_TRACE, "%"PRId64" %d/%d %"PRId64"\n", ts,
                st->time_base.num, st->time_base.den, ast->frame_offset);
        if (ts < best_ts) {
            best_ts           = ts;
            best_st           = st;
            best_stream_index = i;
        }
    }
    if (!best_st)
        return AVERROR_EOF;

    best_ast = best_st->priv_data;
    best_ts  = best_ast->frame_offset;
    if (best_ast->remaining) {
        i = av_index_search_timestamp(best_st,
                                      best_ts,
                                      AVSEEK_FLAG_ANY |
                                      AVSEEK_FLAG_BACKWARD);
    } else {
        i = av_index_search_timestamp(best_st, best_ts, AVSEEK_FLAG_ANY);
        if (i >= 0)
            best_ast->frame_offset = best_st->index_entries[i].timestamp;
    }

    if (i >= 0) {
        int64_t pos = best_st->index_entries[i].pos;
        pos += best_ast->packet_size - best_ast->remaining;
        if (avio_seek(s->pb, pos + 8, SEEK_SET) < 0)
          return AVERROR_EOF;

        av_assert0(best_ast->remaining <= best_ast->packet_size);

        avi->stream_index = best_stream_index;
        if (!best_ast->remaining)
            best_ast->packet_size =
            best_ast->remaining   = best_st->index_entries[i].size;
    }
    else
        return AVERROR_EOF;

    return 0;
}
