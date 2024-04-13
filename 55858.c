static int rm_read_seek(AVFormatContext *s, int stream_index,
                        int64_t pts, int flags)
{
    RMDemuxContext *rm = s->priv_data;

    if (ff_seek_frame_binary(s, stream_index, pts, flags) < 0)
        return -1;
    rm->audio_pkt_cnt = 0;
    return 0;
}
