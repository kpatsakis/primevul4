static void seek_subtitle(AVStream *st, AVStream *st2, int64_t timestamp)
{
    AVIStream *ast2 = st2->priv_data;
    int64_t ts2     = av_rescale_q(timestamp, st->time_base, st2->time_base);
    av_packet_unref(&ast2->sub_pkt);
    if (avformat_seek_file(ast2->sub_ctx, 0, INT64_MIN, ts2, ts2, 0) >= 0 ||
        avformat_seek_file(ast2->sub_ctx, 0, ts2, ts2, INT64_MAX, 0) >= 0)
        ff_read_packet(ast2->sub_ctx, &ast2->sub_pkt);
}
