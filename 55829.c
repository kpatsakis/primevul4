static int mv_read_seek(AVFormatContext *avctx, int stream_index,
                        int64_t timestamp, int flags)
{
    MvContext *mv = avctx->priv_data;
    AVStream *st = avctx->streams[stream_index];
    int frame, i;

    if ((flags & AVSEEK_FLAG_FRAME) || (flags & AVSEEK_FLAG_BYTE))
        return AVERROR(ENOSYS);

    if (!(avctx->pb->seekable & AVIO_SEEKABLE_NORMAL))
        return AVERROR(EIO);

    frame = av_index_search_timestamp(st, timestamp, flags);
    if (frame < 0)
        return AVERROR_INVALIDDATA;

    for (i = 0; i < avctx->nb_streams; i++)
        mv->frame[i] = frame;
    return 0;
}
