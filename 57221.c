static int guess_ni_flag(AVFormatContext *s)
{
    int i;
    int64_t last_start = 0;
    int64_t first_end  = INT64_MAX;
    int64_t oldpos     = avio_tell(s->pb);

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st = s->streams[i];
        int n        = st->nb_index_entries;
        unsigned int size;

        if (n <= 0)
            continue;

        if (n >= 2) {
            int64_t pos = st->index_entries[0].pos;
            unsigned tag[2];
            avio_seek(s->pb, pos, SEEK_SET);
            tag[0] = avio_r8(s->pb);
            tag[1] = avio_r8(s->pb);
            avio_rl16(s->pb);
            size = avio_rl32(s->pb);
            if (get_stream_idx(tag) == i && pos + size > st->index_entries[1].pos)
                last_start = INT64_MAX;
            if (get_stream_idx(tag) == i && size == st->index_entries[0].size + 8)
                last_start = INT64_MAX;
        }

        if (st->index_entries[0].pos > last_start)
            last_start = st->index_entries[0].pos;
        if (st->index_entries[n - 1].pos < first_end)
            first_end = st->index_entries[n - 1].pos;
    }
    avio_seek(s->pb, oldpos, SEEK_SET);

    if (last_start > first_end)
        return 1;

    return check_stream_max_drift(s);
}
