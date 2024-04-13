static int avi_read_close(AVFormatContext *s)
{
    int i;
    AVIContext *avi = s->priv_data;

    for (i = 0; i < s->nb_streams; i++) {
        AVStream *st   = s->streams[i];
        AVIStream *ast = st->priv_data;
        if (ast) {
            if (ast->sub_ctx) {
                av_freep(&ast->sub_ctx->pb);
                avformat_close_input(&ast->sub_ctx);
            }
            av_freep(&ast->sub_buffer);
            av_packet_unref(&ast->sub_pkt);
        }
    }

    av_freep(&avi->dv_demux);

    return 0;
}
