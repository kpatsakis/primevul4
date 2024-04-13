static int cine_read_seek(AVFormatContext *avctx, int stream_index, int64_t timestamp, int flags)
{
    CineDemuxContext *cine = avctx->priv_data;

    if ((flags & AVSEEK_FLAG_FRAME) || (flags & AVSEEK_FLAG_BYTE))
        return AVERROR(ENOSYS);

    if (!(avctx->pb->seekable & AVIO_SEEKABLE_NORMAL))
        return AVERROR(EIO);

    cine->pts = timestamp;
    return 0;
}
