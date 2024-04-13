static void clean_index(AVFormatContext *s)
{
    int i;
    int64_t j;

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st   = s->streams[i];
        AVIStream *ast = st->priv_data;
        int n          = st->nb_index_entries;
        int max        = ast->sample_size;
        int64_t pos, size, ts;

        if (n != 1 || ast->sample_size == 0)
            continue;

        while (max < 1024)
            max += max;

        pos  = st->index_entries[0].pos;
        size = st->index_entries[0].size;
        ts   = st->index_entries[0].timestamp;

        for (j = 0; j < size; j += max)
            av_add_index_entry(st, pos + j, ts + j, FFMIN(max, size - j), 0,
                               AVINDEX_KEYFRAME);
    }
}
