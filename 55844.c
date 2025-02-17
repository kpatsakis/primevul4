ff_rm_retrieve_cache (AVFormatContext *s, AVIOContext *pb,
                      AVStream *st, RMStream *ast, AVPacket *pkt)
{
    RMDemuxContext *rm = s->priv_data;

    av_assert0 (rm->audio_pkt_cnt > 0);

    if (ast->deint_id == DEINT_ID_VBRF ||
        ast->deint_id == DEINT_ID_VBRS) {
        int ret = av_get_packet(pb, pkt, ast->sub_packet_lengths[ast->sub_packet_cnt - rm->audio_pkt_cnt]);
        if (ret < 0)
            return ret;
    } else {
        int ret = av_new_packet(pkt, st->codecpar->block_align);
        if (ret < 0)
            return ret;
        memcpy(pkt->data, ast->pkt.data + st->codecpar->block_align * //FIXME avoid this
               (ast->sub_packet_h * ast->audio_framesize / st->codecpar->block_align - rm->audio_pkt_cnt),
               st->codecpar->block_align);
    }
    rm->audio_pkt_cnt--;
    if ((pkt->pts = ast->audiotimestamp) != AV_NOPTS_VALUE) {
        ast->audiotimestamp = AV_NOPTS_VALUE;
        pkt->flags = AV_PKT_FLAG_KEY;
    } else
        pkt->flags = 0;
    pkt->stream_index = st->index;

    return rm->audio_pkt_cnt;
}
