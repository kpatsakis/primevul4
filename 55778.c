static int cine_read_packet(AVFormatContext *avctx, AVPacket *pkt)
{
    CineDemuxContext *cine = avctx->priv_data;
    AVStream *st = avctx->streams[0];
    AVIOContext *pb = avctx->pb;
    int n, size, ret;

    if (cine->pts >= st->duration)
        return AVERROR_EOF;

    avio_seek(pb, st->index_entries[cine->pts].pos, SEEK_SET);
    n = avio_rl32(pb);
    if (n < 8)
        return AVERROR_INVALIDDATA;
    avio_skip(pb, n - 8);
    size = avio_rl32(pb);

    ret = av_get_packet(pb, pkt, size);
    if (ret < 0)
        return ret;

    pkt->pts = cine->pts++;
    pkt->stream_index = 0;
    pkt->flags |= AV_PKT_FLAG_KEY;
    return 0;
}
