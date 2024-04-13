static int rm_read_header_old(AVFormatContext *s)
{
    RMDemuxContext *rm = s->priv_data;
    AVStream *st;

    rm->old_format = 1;
    st = avformat_new_stream(s, NULL);
    if (!st)
        return -1;
    st->priv_data = ff_rm_alloc_rmstream();
    if (!st->priv_data)
        return AVERROR(ENOMEM);
    return rm_read_audio_stream_info(s, s->pb, st, st->priv_data, 1);
}
