static int rl2_read_seek(AVFormatContext *s, int stream_index, int64_t timestamp, int flags)
{
    AVStream *st = s->streams[stream_index];
    Rl2DemuxContext *rl2 = s->priv_data;
    int i;
    int index = av_index_search_timestamp(st, timestamp, flags);
    if(index < 0)
        return -1;

    rl2->index_pos[stream_index] = index;
    timestamp = st->index_entries[index].timestamp;

    for(i=0; i < s->nb_streams; i++){
        AVStream *st2 = s->streams[i];
        index = av_index_search_timestamp(st2,
                    av_rescale_q(timestamp, st->time_base, st2->time_base),
                    flags | AVSEEK_FLAG_BACKWARD);

        if(index < 0)
            index = 0;

        rl2->index_pos[i] = index;
    }

    return 0;
}
