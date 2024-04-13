static int check_stream_max_drift(AVFormatContext *s)
{
    int64_t min_pos, pos;
    int i;
    int *idx = av_mallocz_array(s->nb_streams, sizeof(*idx));
    if (!idx)
        return AVERROR(ENOMEM);
    for (min_pos = pos = 0; min_pos != INT64_MAX; pos = min_pos + 1LU) {
        int64_t max_dts = INT64_MIN / 2;
        int64_t min_dts = INT64_MAX / 2;
        int64_t max_buffer = 0;

        min_pos = INT64_MAX;

        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            AVIStream *ast = st->priv_data;
            int n = st->nb_index_entries;
            while (idx[i] < n && st->index_entries[idx[i]].pos < pos)
                idx[i]++;
            if (idx[i] < n) {
                int64_t dts;
                dts = av_rescale_q(st->index_entries[idx[i]].timestamp /
                                   FFMAX(ast->sample_size, 1),
                                   st->time_base, AV_TIME_BASE_Q);
                min_dts = FFMIN(min_dts, dts);
                min_pos = FFMIN(min_pos, st->index_entries[idx[i]].pos);
            }
        }
        for (i = 0; i < s->nb_streams; i++) {
            AVStream *st = s->streams[i];
            AVIStream *ast = st->priv_data;

            if (idx[i] && min_dts != INT64_MAX / 2) {
                int64_t dts;
                dts = av_rescale_q(st->index_entries[idx[i] - 1].timestamp /
                                   FFMAX(ast->sample_size, 1),
                                   st->time_base, AV_TIME_BASE_Q);
                max_dts = FFMAX(max_dts, dts);
                max_buffer = FFMAX(max_buffer,
                                   av_rescale(dts - min_dts,
                                              st->codecpar->bit_rate,
                                              AV_TIME_BASE));
            }
        }
        if (max_dts - min_dts > 2 * AV_TIME_BASE ||
            max_buffer > 1024 * 1024 * 8 * 8) {
            av_free(idx);
            return 1;
        }
    }
    av_free(idx);
    return 0;
}
